////////////////////////////////////////
// Skin.cpp
////////////////////////////////////////

#include "Skin.h"

////////////////////////////////////////////////////////////////////////////////

Skin::Skin(const char *file, Skeleton* skell)
{
	this->Skell = skell;
	Load(file);
	SetBuffers();
}

////////////////////////////////////////////////////////////////////////////////

Skin::~Skin()
{
	for (Vertex* v : this->Vertices) delete(v);
	for (Triangle* t : this->Triangles) delete(t);
}

////////////////////////////////////////////////////////////////////////////////

void Skin::Update()
{
	// Compute skinning matrix for each joint
	// Loop through vertices and compute blended position and normal

	this->BlendedPos.clear();
	this->BlendedNorm.clear();

	int negC = 0;

	for (Vertex* v : this->Vertices)
	{
		glm::vec4 currPos = glm::vec4(v->GetPos(), 1.0f);
		glm::vec4 currNorm = glm::vec4(v->GetNorm(), 0.0f);
		glm::vec4 newPos = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		glm::vec4 newNorm = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
		std::vector<int> currBI = v->GetBindingIndices();
		std::vector<float> currBW = v->GetBindingWeights();

		for (unsigned int i = 0; i < currBI.size(); i++)
		{
			float currWeight = currBW[i];
			int currInd = currBI[i];
			glm::mat4x4 currBin = this->BInverses[currInd];
			glm::mat4x4 currWorld = this->Skell->GetJointWorldMtx(currInd);

			newPos += currWeight * currWorld * currBin * currPos;
			newNorm += currWeight * currWorld * currBin * currNorm;
		}

		newNorm = glm::vec4(glm::normalize(glm::vec3(newNorm)), 0.0f);
		v->SetBlendedPos(glm::vec3(newPos));
		v->SetBlendedNorm(glm::vec3(newNorm));
		
		this->BlendedPos.push_back(glm::vec3(newPos));
		this->BlendedNorm.push_back(glm::vec3(newNorm));
	}

	// Rebind VBOs
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, this->BlendedPos.size() * sizeof(glm::vec3), this->BlendedPos.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, NormalBuffer);
	glBufferData(GL_ARRAY_BUFFER, this->BlendedNorm.size() * sizeof(glm::vec3), this->BlendedNorm.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

////////////////////////////////////////////////////////////////////////////////

void Skin::Draw(const glm::mat4x4 &viewProjMtx, uint shader)
{
	// Set GL matrix state to identity (world)
	glm::mat4x4 modelMtx = this->Skell->GetWorldMtx();

	// Loop through triangles and draw using world space positions and normals

	// Set up shader
	glUseProgram(shader);
	glUniformMatrix4fv(glGetUniformLocation(shader, "ModelMtx"), 1, false, (float*)&modelMtx);

	glm::mat4x4 mvpMtx = viewProjMtx * modelMtx;
	glUniformMatrix4fv(glGetUniformLocation(shader, "ModelViewProjMtx"), 1, false, (float*)&mvpMtx);

	// Set up state
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
	uint posLoc = 0;
	glEnableVertexAttribArray(posLoc);
	glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, NormalBuffer);
	uint normLoc = 1;
	glEnableVertexAttribArray(normLoc);
	glVertexAttribPointer(normLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBuffer);

	// Draw geometry
	glDrawElements(GL_TRIANGLES, (GLsizei)this->Indices.size(), GL_UNSIGNED_INT, 0);

	// Clean up state
	glDisableVertexAttribArray(normLoc);
	glDisableVertexAttribArray(posLoc);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glUseProgram(0);
}

////////////////////////////////////////////////////////////////////////////////

bool Skin::Load(const char *file)
{
	int count = 0;
	int numVerts;
	int numTriangles;
	int numJoints;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<std::vector<int>> bindingIndices;
	std::vector<std::vector<float>> bindingWeights;

	Tokenizer token;
	token.Open(file);

	// Parse positions, [x] [y] [z]
	token.FindToken("positions");
	numVerts = token.GetInt();
	token.FindToken("{");
	while (true) {
		if (count == numVerts) break;

		glm::vec3 currVec;
		currVec.x = token.GetFloat();
		currVec.y = token.GetFloat();
		currVec.z = token.GetFloat();
		vertices.push_back(currVec);
		count++;

		BlendedPos.push_back(currVec);
	}
	std::cout << count << " vertices found." << std::endl;
	count = 0;

	// Parse normals, [x] [y] [z]
	token.FindToken("normals");
	token.FindToken("{");
	while (true) {
		if (count == numVerts) break;

		glm::vec3 currVec;
		currVec.x = token.GetFloat();
		currVec.y = token.GetFloat();
		currVec.z = token.GetFloat();
		normals.push_back(currVec);
		count++;

		BlendedNorm.push_back(currVec);
	}
	std::cout << count << " normals found." << std::endl;
	count = 0;

	// Parse skin weights, [numattachments] [jointX] [weightX]
	token.FindToken("skinweights");
	token.FindToken("{");
	while (true) {
		if (count == numVerts) break;

		std::vector<int> currIndices;
		std::vector<float> currWeights;
		int numJoints = token.GetInt();
		for (int i = 0; i < numJoints; i++)
		{
			currIndices.push_back(token.GetInt());
			currWeights.push_back(token.GetFloat());
		}
		bindingIndices.push_back(currIndices);
		bindingWeights.push_back(currWeights);
		count++;
	}
	std::cout << count << " skin weights found." << std::endl;
	count = 0;

	// Create vertices
	for (int i = 0; i < numVerts; i++)
	{
		Vertex* currVert = new Vertex(vertices[i], normals[i]);
		currVert->SetBindingIndices(bindingIndices[i]);
		currVert->SetBindingWeights(bindingWeights[i]);
		this->Vertices.push_back(currVert);
	}

	// Parse triangles, [vertex0] [vertex1] [vertex2]
	token.FindToken("triangles");
	numTriangles = token.GetInt();
	token.FindToken("{");
	while (true) {
		if (count == numTriangles) break;

		int v0 = token.GetInt();
		int v1 = token.GetInt();
		int v2 = token.GetInt();
		Triangle* currTri = new Triangle(this->Vertices[v0], this->Vertices[v1], this->Vertices[v2]);
		std::vector<int> currInd;
		currInd.push_back(v0);
		currInd.push_back(v1);
		currInd.push_back(v2);
		currTri->SetIndices(currInd);
		this->Triangles.push_back(currTri);
		count++;

		Indices.push_back(v0);
		Indices.push_back(v1);
		Indices.push_back(v2);
	}
	std::cout << count << " triangles found." << std::endl;
	count = 0;

	// Parse bindings, matrix {}
	token.FindToken("bindings");
	numJoints = token.GetInt();
	token.FindToken("{");
	while (true) {
		if (count == numJoints) break;

		token.FindToken("matrix");
		token.FindToken("{");
		glm::mat4x4 currMat = glm::mat4x4(1);

		// [ax] [ay] [az]
		currMat[0][0] = token.GetFloat();
		currMat[0][1] = token.GetFloat();
		currMat[0][2] = token.GetFloat();

		// [bx] [by] [bz]
		currMat[1][0] = token.GetFloat();
		currMat[1][1] = token.GetFloat();
		currMat[1][2] = token.GetFloat();

		// [cx] [cy] [cz]
		currMat[2][0] = token.GetFloat();
		currMat[2][1] = token.GetFloat();
		currMat[2][2] = token.GetFloat();

		// [dx] [dy] [dz]
		currMat[3][0] = token.GetFloat();
		currMat[3][1] = token.GetFloat();
		currMat[3][2] = token.GetFloat();

		this->Bindings.push_back(currMat);
		count++;
	}
	std::cout << count << " matrices found." << std::endl;
	count = 0;

	// Compute inverses of binding matrices
	for (glm::mat4x4 m : this->Bindings) this->BInverses.push_back(glm::inverse(m));

	// Finish
	token.Close();
	return true;
}

////////////////////////////////////////////////////////////////////////////////

void Skin::SetBuffers()
{
	// Initialize buffers
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VertexBuffer);
	glGenBuffers(1, &NormalBuffer);
	glGenBuffers(1, &IndexBuffer);

	// Bind VAO for vertex/normal buffers
	glBindVertexArray(VAO);

	// Store vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, this->BlendedPos.size() * sizeof(glm::vec3), this->BlendedPos.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Store normal buffer
	glBindBuffer(GL_ARRAY_BUFFER, NormalBuffer);
	glBufferData(GL_ARRAY_BUFFER, this->BlendedNorm.size() * sizeof(glm::vec3), this->BlendedNorm.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Store index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->Indices.size() * sizeof(unsigned int), &this->Indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

////////////////////////////////////////////////////////////////////////////////