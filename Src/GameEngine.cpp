/**
* @file GameEngine.cpp
*/
#include "GameEngine.h"
#include "GLFWEW.h"
#include "Audio.h"
#include "Constants.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

///頂点データ型
struct Vertex {
	glm::vec3 position;     ///座標
	glm::vec4 color;        ///色
	glm::vec2 texCoord;     ///テクスチャ座標
	glm::vec3 normal;       ///法線
};

float length = sqrt(2) * 0.5;
///頂点データ
const Vertex vertices[] = {
	{ { -0.5f, -0.3f,  0.5f } ,{ 1.0f, 1.0f, 1.0f, 1.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } },
	{ { 0.3f, -0.3f,  0.5f } ,{ 1.0f, 1.0f, 1.0f, 1.0f },{ 1.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } },
	{ { 0.3f,  0.5f,  0.5f } ,{ 1.0f, 1.0f, 1.0f, 1.0f },{ 1.0f, 1.0f },{ 0.0f, 0.0f, 1.0f } },
	{ { -0.5f,  0.5f,  0.5f } ,{ 1.0f, 1.0f, 1.0f, 1.0f },{ 0.0f, 1.0f },{ 0.0f, 0.0f, 1.0f } },

	{ { -0.3f,  0.3f,  0.1f } ,{ 0.0f, 0.0f, 1.0f, 1.0f },{ 0.0f, 1.0f },{ 0.0f, 0.0f, 1.0f } },
	{ { -0.3f, -0.5f,  0.1f } ,{ 0.0f, 1.0f, 1.0f, 1.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } },
	{ { 0.5f, -0.5f,  0.1f } ,{ 0.0f, 0.0f, 1.0f, 1.0f },{ 1.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } },
	{ { 0.5f, -0.5f,  0.1f } ,{ 1.0f, 0.0f, 0.0f, 1.0f },{ 1.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } },
	{ { 0.5f,  0.3f,  0.1f } ,{ 1.0f, 1.0f, 0.0f, 1.0f },{ 1.0f, 1.0f },{ 0.0f, 0.0f, 1.0f } },
	{ { -0.3f,  0.3f,  0.1f } ,{ 1.0f, 0.0f, 0.0f, 1.0f },{ 0.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f } },

	{ { -1.0f, -1.0f, 0.0f } ,{ 1.0f, 1.0f, 1.0f, 1.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } },
	{ { 1.0f, -1.0f, 0.0f } ,{ 1.0f, 1.0f, 1.0f, 1.0f },{ 1.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } },
	{ { 1.0f,  1.0f, 0.0f } ,{ 1.0f, 1.0f, 1.0f, 1.0f },{ 1.0f, 1.0f },{ 0.0f, 0.0f, 1.0f } },
	{ { -1.0f,  1.0f, 0.0f } ,{ 1.0f, 1.0f, 1.0f, 1.0f },{ 0.0f, 1.0f },{ 0.0f, 0.0f, 1.0f } },
};

///インデックスデータ
const GLuint indices[] = {
	0, 1, 2, 2, 3, 0,

	4, 5, 6, 7, 8, 9,

	10, 11, 12, 12, 13, 10,

};




/**
* 部分描画データ
*/
struct RenderingPart {
	GLsizei size;       ///描画するインデックス数
	GLvoid* offset;     ///描画開始インデックスのバイトオフセット
};

/**
* RenderingPart を作成する
*
* @param   size    描画するインデックス数
* @param   offset  描画開始インデックス数のオフセット（インデックス単位）
*
* @return  作成した部分描画データ
*/
RenderingPart MakeRenderingPart(GLsizei size, GLsizei offset) {
	return { size, reinterpret_cast<GLvoid*>(offset * sizeof(GLuint)) };
}

/**
* 部分描画データリスト
*/
static const RenderingPart renderingParts[] = {
	MakeRenderingPart(12, 0),
	MakeRenderingPart(6, 12),
};

/**
*  Vertex Buffer Onject を作成する
*
* @param   size    頂点データのサイズ
* @param   data    頂点データへのポインタ
*
* @return  作成した VBO
*/
GLuint CreateVBO(GLsizeiptr size, const GLvoid* data) {
	GLuint vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	return vbo;
}


///インデックスデータを使うことでおんなじデータを複数作らなくてすむ
/**
* Index Buffer Objectを作成する
*
* @param   size    インデックスデータのサイズ
* @param   data    インデックスデータへのポインタ
*
* @return  作成したIBO
*/
GLuint CreateIBO(GLsizeiptr size, const GLvoid* data) {
	GLuint ibo = 0;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	return ibo;
}

/**
*  頂点アトリビュートを設定する
*
* @param   index   頂点アトリビュートのインデックス
* @param   cls     頂点データ型名
* @param   mbr     頂点アトリビュートに設定する cls のメンバ変数名
*/
#define SetVertexAttribPointer( index, cls, mbr ) SetVertexAttribPointerI ( \
index, \
sizeof( cls::mbr ) / sizeof( float ), \
sizeof(cls),\
reinterpret_cast<GLvoid*>(offsetof(cls,mbr)))

void SetVertexAttribPointerI(GLuint index, GLint size, GLsizei stride, const GLvoid* pointer) {
	glEnableVertexAttribArray(index);
	glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, stride, pointer);
}

/**
*  Vertex Array Object を作成する
*
* @param   vbo VAO に関連づけられる VBO
* @param   ibo VAO に関連づけられる IBO
*
* @return  作成した VAO
*/
GLuint CreateVAO(GLuint vbo, GLuint ibo) {
	GLuint vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	SetVertexAttribPointer(0, Vertex, position);
	SetVertexAttribPointer(1, Vertex, color);
	SetVertexAttribPointer(2, Vertex, texCoord);
	SetVertexAttribPointer(3, Vertex, normal);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBindVertexArray(0);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);
	return vao;
}


/**
* ゲームエンジンのインスタンスを取得する
*
* @return ゲームエンジンのインスタンス
*/
GameEngine & GameEngine::Instance(){
	static GameEngine instance;
	return instance;
}

/**
* ゲームエンジンを初期化する
*
* @param w	ウィンドウの描画範囲の幅（ピクセル）
* @param h	ウィンドウの描画範囲の高さ（ピクセル）
* @param title	ウィンドウタイトル（UTF-8の0終端文字列）
*
* @retval true	初期化成功
* @retval false 初期化失敗
*
* Run関数を呼び出す前に、一度だけ呼び出しておく必要がある。
* 一度初期化に成功すると、以後の呼び出しでは何もせずにtrueを返す
*/
bool GameEngine::Init(int w, int h, const char * title)
{
	if (isInitialized) {
		return true;
	}
	if (!GLFWEW::Window::Instance().Init(w, h, title)) {
		return false;
	}

	//<-----------------ここに初期化処理を追加する----------------->
	vbo = CreateVBO(sizeof(vertices), vertices);
	ibo = CreateIBO(sizeof(indices), indices);
	vao = CreateVAO(vbo, ibo);
	uboLight = UniformBuffer::Create(sizeof(Uniform::LightData), 1, "LightData");
	uboPostEffect = UniformBuffer::Create(sizeof(Uniform::PostEffectData), 2, "PostEffectData");
	offscreen = OffscreenBuffer::Create(960, 600, GL_RGBA16F);
	for (int i = 0, scale = 4; i < bloomBufferCount; ++i, scale *= 4) {
		const int w = 960 / scale;
		const int h = 600 / scale;
		offBloom[i] = OffscreenBuffer::Create(w, h, GL_RGBA16F);
		if (!offBloom[i]) {
			return false;
		}
	}
	if (!vbo || !ibo || !vao || !uboLight || !uboPostEffect || !offscreen) {
		std::cerr << "ERROR : GameEngineの初期化に失敗" << std::endl;
		return false;
	}

	static const char* const shaderNameList[][3] = {
		{"Tutorial", "Res/Tutorial.vert", "Res/Tutorial.frag"},
		{"ColorFilter", "Res/ColorFilter.vert", "Res/ColorFilter.frag"},
		{"NonLighting", "Res/NonLighting.vert", "Res/NonLighting.frag"},
		{"HiLumExtract", "Res/TexCoord.vert", "Res/HiLumExtraction.frag"},
		{"Shrink","Res/TexCoord.vert", "Res/Shrink.frag"},
		{"Blur3x3", "Res/TexCoord.vert", "Res/Blur3x3.frag"},
	};
	shaderMap.reserve(sizeof(shaderNameList) / sizeof(shaderNameList[0]));
	for (auto& e : shaderNameList) {
		Shader::ProgramPtr program = Shader::Program::Create(e[1], e[2]);
		if (!program) {
			continue;
		}
		shaderMap.insert(std::make_pair(std::string(e[0]), program));
	}
	shaderMap["Tutorial"]->UniformBlockBinding("VertexData", 0);
	shaderMap["Tutorial"]->UniformBlockBinding("LightData", 1);
	shaderMap["ColorFilter"]->UniformBlockBinding("PostEffectData", 2);
	shaderMap["NonLighting"]->UniformBlockBinding("VertexData", 0);

	meshBuffer = Mesh::Buffer::Create(10 * 1024, 30 * 1024);
	if (!meshBuffer) {
		std::cerr << "ERROR : GameEngineの初期化に失敗" << std::endl;
		return false;
	}

	entityBuffer = Entity::Buffer::Create(1024, sizeof(Uniform::VertexData), 0, "VertexData");
	if (!entityBuffer) {
		std::cerr << "ERROR : GameEngineの初期化に失敗" << std::endl;
		return false;
	}

	rand.seed(std::random_device()());
	fontRenderer.Init(1024, glm::vec2(800, 600));
	isInitialized = true;

	return true;
}

/**
* ゲームを実行する
*/
void GameEngine::Run(){
	GLFWEW::Window& window = GLFWEW::Window::Instance();
	double prevTime = glfwGetTime();

	while (!window.ShouldClose()) {
		const double curTime = glfwGetTime();
		const double delta = curTime - prevTime;
		prevTime = curTime;
		window.UpdateGamePad();
		Update(glm::min(0.25,delta));
		Render();
		window.SwapBuffers();
	}
}

/**
* 状態更新関数を設定する
*
* @param func 設定する更新関数
*/
void GameEngine::UpdateFunc(const UpdateFuncType & func){
	updateFunc = func;
}

/**
* 状態更新関数を取得する
*
* @return 設定されている更新関数
*/
const GameEngine::UpdateFuncType& GameEngine::UpdateFunc() const{
	return updateFunc;
}

/**
* テクスチャを読み込む
*
* @param filename	テクスチャファイル名
*
* @retval true	読み込み成功
* @retval false 読み込み失敗
*/
bool GameEngine::LoadTextureFromFile(const char * filename){
	const auto itr = textureBuffer.find(filename);
	if (itr != textureBuffer.end()) {
		return true;
	}
	TexturePtr texture = Texture::LoadFromFile(filename);
	if (!texture) {
		return false;
	}
	textureBuffer.insert(std::make_pair(std::string(filename), texture));
	return true;
}

/**
* メッシュを読み込む
*
* @param filename	メッシュファイル名
*
* @retval true	読み込み成功
* @retval fales 読み込み失敗
*/
bool GameEngine::LoadMeshFromFile(const char * filename)
{
	return meshBuffer->LoadMeshFromFile(filename);
}

/**
* エンティティを追加する
*
* @param groupId	エンティティのグループID
* @param pos		エンティティの場所
* @param meshName	エンティティの表示に使用するメッシュ名
* @param texName	エンティティの表示に使用するテクスチャファイル名
* @param normalName	エンティティの表示に使用するノーマルマップテクスチャ名
* @param func		エンティティの状態を更新する関数（または関数オブジェクト）
* @param shader		エンティティの表示に使うシェーダ名
*
* @return	追加したエンティティへのポインタ
*			これ以上エンティティを追加できない場合はnullptrが返される
*			回転や拡大率はこのポインタ経由で設定する
*			なお、このポインタをアプリケーション側で保持する必要はない
*/
Entity::Entity * GameEngine::AddEntity(int groupId, const glm::vec3 & pos, const char * meshName, const char * texName, const char * normalName, Entity::Entity::UpdateFuncType func, const char * shader){
	decltype(shaderMap)::const_iterator itr = shaderMap.end();
	if (shader) {
		itr = shaderMap.find(shader);
	}
	if (itr == shaderMap.end()) {
		itr = shaderMap.find("Tutorial");
		if (itr == shaderMap.end()) {
			return nullptr;
		}
	}
	const Mesh::MeshPtr& mesh = meshBuffer->GetMesh(meshName);
	TexturePtr tex[2];
	tex[0] = GetTexture(texName);
	if (normalName) {
		tex[1] = GetTexture(normalName);
	}
	else {
		tex[1] = GetTexture(NORMALMAP_FILE_DUMMY);
	}
	return entityBuffer->AddEntity(groupId, pos, mesh, tex, itr->second, func);
}

/**
* エンティティを追加する
*
* @param groupId	エンティティのグループID
* @param pos		エンティティの場所
* @param meshName	エンティティの表示に使用するメッシュ名
* @param texName	エンティティの表示に使用するテクスチャファイル名
* @param func		エンティティの状態を更新する関数（または関数オブジェクト）
* @param shader		エンティティの表示に使うシェーダ名
*
* @return	追加したエンティティへのポインタ
*			これ以上エンティティを追加できない場合はnullptrが返される
*			回転や拡大率はこのポインタ経由で設定する
*			なお、このポインタをアプリケーション側で保持する必要はない
*/
Entity::Entity * GameEngine::AddEntity(int groupId, const glm::vec3 & pos, const char * meshName, const char * texName,
	Entity::Entity::UpdateFuncType func, const char* shader){
	return AddEntity(groupId, pos, meshName, texName, nullptr, func, shader);
}

/**
* エンティティを削除する
*
* @param 削除するエンティティのポインタ
*/
void GameEngine::RemoveEntity(Entity::Entity* e){
	entityBuffer->RemoveEntity(e);
}

/**
* ライトを設定する
*
* @param index	設定するライトのインデックス
* @param light	ライトデータ
*/
void GameEngine::Light(int index, const Uniform::PointLight & light){
	if (index < 0 || index >= Uniform::maxLightCount) {
		std::cerr << "WARNING : '" << index << "'は不正なライトインデックスです" << std::endl;
		return;
	}
	lightData.light[index] = light;
}

/**
* ライトを取得する
*
* @param index	取得するライトのインデックス
*
* @return ライトデータ
*/
const Uniform::PointLight & GameEngine::Light(int index) const{
	if (index < 0 || index >= Uniform::maxLightCount) {
		std::cerr << "WARNING : '" << index << "'は不正なライトインデックスです" << std::endl;
		static const Uniform::PointLight dummy;
		return dummy;
	}
	return lightData.light[index];

}

/**
* 環境光を設定する
*
* @param color	環境光の明るさ
*/
void GameEngine::AmbientLight(const glm::vec4 & color){
	lightData.ambinetColor = color;
}

/**
* 環境光を取得する
*
* @return 環境光の明るさ
*/
const glm::vec4 & GameEngine::AmbientLight() const{
	return lightData.ambinetColor;
}

/**
* 視点の位置と姿勢を設定する
*
* @param cam 設定するカメラデータ
*/
void GameEngine::Camera(const CameraData & cam){
	camera = cam;
}

/**
* 視点の位置と姿勢を取得する
*
* @return カメラデータ
*/
const GameEngine::CameraData& GameEngine::Camera() const{
	return camera;
}

/**
* 乱数オブジェクトを取得する
*
* @return 乱数オブジェクト
*/
std::mt19937& GameEngine::Rand(){
	return rand;
}

const GamePad & GameEngine::GetGamePad() const{
	return GLFWEW::Window::Instance().GetGamePad();
}

/**
* @copydoc Audio::Initialize
*/
bool GameEngine::InitAudio(const char * acfPath, const char * acbPath, const char * awbPath, const char * dspBusName){
	return Audio::Initialize(acfPath, acbPath, awbPath, dspBusName);
}

/**
* @copydoc Audio::Play
*/
void GameEngine::PlayAudio(int playerId, int cueId){
	Audio::Play(playerId, cueId);
}

/**
* @copydoc Audio::Stop
*/
void GameEngine::StopAudio(int playerId){
	Audio::Stop(playerId);
}

/**
* 衝突解決ハンドラを設定する
*
* @param gid0		衝突対象のグループID
* @param gid1		衝突対象のグループID
* @param handler	衝突解決ハンドラ
*
* 衝突が発生し衝突解決ハンドラが呼び出されるとき、より小さいグループIDをもつエンティティから先に渡される
* ここで指定したグループIDの順序とは無関係であることに注意
* ex)
*	CollisionHandler(10, 1, Func)
*   というコードでハンドラを登録したとすると衝突が発生すると
*	Func(グループID = 1のエンティティ、グループID=10のエンティティ)
*	のように呼び出される
*/
void GameEngine::CollisionHandler(int gid0, int gid1, Entity::CollisionHandlerType handler){
	entityBuffer->CollisionHandler(gid0, gid1, handler);
}

/**
* 衝突解決ハンドラを取得する
*
* @param gid0	衝突対象のグループID
* @param gid1	衝突対象のグループID
*
* @return 衝突解決ハンドラ
*/
const Entity::CollisionHandlerType& GameEngine::CollisionHandler(int gid0, int gid1) const{
	return entityBuffer->CollisionHandler(gid0, gid1);
}

/**
* 全ての衝突解決ハンドラを削除する
*/
void GameEngine::ClearCollisionHandlerList(){
	entityBuffer->ClearCollisionHandlerList();
}

/**
* デストラクタ
*/
GameEngine::~GameEngine(){
	updateFunc = nullptr;
	Audio::Destroy();
	if (vao) {
		glDeleteVertexArrays(1, &vao);
	}
	if (ibo) {
		glDeleteBuffers(1, &ibo);
	}
	if (vbo) {
		glDeleteBuffers(1, &vbo);
	}
}

/**
* ゲームの状態を更新する
*
* @param delta	前回の更新からの経過時間
*/
void GameEngine::Update(double delta){
	fontRenderer.MapBuffer();
	if (updateFunc) {
		updateFunc(delta);
	}
	//<---------------ここに更新処理を追加する------------------->
	const glm::mat4x4 matProj =
		glm::perspective(glm::radians(45.0f), 960.0f / 600.0f, 0.1f, 200.0f);
	const glm::mat4x4 matView = glm::lookAt(camera.position, camera.target, camera.up);
	entityBuffer->Update(delta, matView, matProj);
	fontRenderer.UnmapBuffer();
}

/**
* ゲームの状態を描画する
*/
void GameEngine::Render() const{
	//<-------------ここに描画処理を追加する--------------------->
	glBindFramebuffer(GL_FRAMEBUFFER, offscreen->GetFrameBuffer());
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glViewport(0, 0, 960, 600);
	glScissor(0, 0, 960, 600);
	glClearColor(0.1f, 0.3f, 0.5f, 1.0f);
	glClearDepth(1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	uboLight->BufferSubData(&lightData);
	entityBuffer->Draw(meshBuffer);
	glBindVertexArray(vao);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);


	//ブルームをするときはテクスチャの範囲外の指定をGL_CLAMP_TO_BORDERとかに変換しないと範囲外を指定してしまったときにおかしくなる_
	const Shader::ProgramPtr& progHiLumExtract = shaderMap.find("HiLumExtract")->second;
	progHiLumExtract->UseProgram();
	glBindFramebuffer(GL_FRAMEBUFFER, offBloom[0]->GetFrameBuffer());
	glViewport(0, 0, offBloom[0]->Width(), offBloom[0]->Height());
	progHiLumExtract->BindTexture(GL_TEXTURE0, GL_TEXTURE_2D, offscreen->GetTexture());
	glDrawElements(GL_TRIANGLES, renderingParts[1].size, GL_UNSIGNED_INT, renderingParts[1].offset);

	const Shader::ProgramPtr& progShrink = shaderMap.find("Shrink")->second;
	progShrink->UseProgram();
	for (int i = 1; i < bloomBufferCount; ++i) {
		glBindFramebuffer(GL_FRAMEBUFFER, offBloom[i]->GetFrameBuffer());
		glViewport(0, 0, offBloom[i]->Width(), offBloom[i]->Height());
		progShrink->BindTexture(GL_TEXTURE0, GL_TEXTURE_2D, offBloom[i - 1]->GetTexture());
		glDrawElements(GL_TRIANGLES, renderingParts[1].size, GL_UNSIGNED_INT, renderingParts[1].offset);
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	const Shader::ProgramPtr& progBlur3x3 = shaderMap.find("Blur3x3")->second;
	progBlur3x3->UseProgram();
	for (int i = bloomBufferCount - 1; i > 0 ; --i) {
		glBindFramebuffer(GL_FRAMEBUFFER, offBloom[i - 1]->GetFrameBuffer());
		glViewport(0, 0, offBloom[i - 1]->Width(), offBloom[i - 1]->Height());
		progBlur3x3->BindTexture(GL_TEXTURE0, GL_TEXTURE_2D, offBloom[i]->GetTexture());
		glDrawElements(GL_TRIANGLES, renderingParts[1].size, GL_UNSIGNED_INT, renderingParts[1].offset);
	}

	glDisable(GL_BLEND);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 960, 600);
	const Shader::ProgramPtr& progColorFilter = shaderMap.find("ColorFilter")->second;
	progColorFilter->UseProgram();
	Uniform::PostEffectData postEffect;
	uboPostEffect->BufferSubData(&postEffect);
	progColorFilter->BindTexture(GL_TEXTURE0, GL_TEXTURE_2D, offscreen->GetTexture());
	progColorFilter->BindTexture(GL_TEXTURE1, GL_TEXTURE_2D, offBloom[0]->GetTexture());
	glDrawElements(GL_TRIANGLES, renderingParts[1].size, GL_UNSIGNED_INT, renderingParts[1].offset);

	fontRenderer.Draw();
}

