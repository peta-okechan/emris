//
//  main.cpp
//  emris
//
//  Created by peta on 2013/11/24.
//  Copyright (c) 2013 peta.okechan.net. All rights reserved.
//

#define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
#define GLM_FORCE_RADIANS 1
#ifdef EMSCRIPTEN
#define		GL_GLEXT_PROTOTYPES
#endif

#include <iostream>
#include <utility>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <sys/time.h>
#include <GL/glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.hpp"
#include "ObjectData.hpp"
#include "Tetris.hpp"

std::string const version = "0.3.1";
unsigned int const fps = 30;
int const backgroundColor = 0xFF005500;
float const screenWidth = 480.0f;
float const screenHeight = 480.0f;

struct Scene;
struct TitleScene;
struct GameScene;
struct TPSGameScene;
struct GameOverScene;
template<typename T> void SwitchScene();
Scene *currentScene = nullptr;
bool tpsMode = false;

using vec3 = glm::vec3;
using vec4 = glm::vec4;
using mat4 = glm::mat4;


/*
 glm::vec2とTetris::Tetrimino::Coordの互換用クラス
 */
struct vec2 : glm::vec2
{
    vec2()
    : glm::vec2()
    {}
    
    vec2(vec2 const &v)
    : glm::vec2(v.x, v.y)
    {}
    
    vec2(float const x, float const y)
    : glm::vec2(x, y)
    {}
    
    vec2(glm::vec4 const &v)
    : glm::vec2(v)
    {}
    
    vec2(Tetris::Tetrimino::Coord const &c)
    : glm::vec2(c.x, c.y)
    {}
    
    vec2 &operator=(glm::vec2 const &v)
    {
        x = v.x; y = v.y;
        return (*this);
    }
};


/*
 vec2にmat4の変換を適用する
 */
vec2 operator*(mat4 const &m, vec2 const &v)
{
    return vec2(m * vec4(v.x, v.y, 0.0f, 1.0f));
}


/*
 シーンの基底クラス
 */
struct Scene
{
    mat4 projMat;
    mat4 viewMat;
    Shader *shader = nullptr;
    
    Scene()
    : projMat(), viewMat()
    {
        projMat = glm::perspective(glm::pi<float>() / 3.0f, screenWidth / screenHeight, 0.1f, 100.0f);
        viewMat = glm::lookAt(vec3(1, 0, 20), vec3(1, 0, 0), vec3(0, 1, 0));
        shader = CreateDefaultShaderProgram();
    }
    
    virtual ~Scene()
    {
        if (shader) delete shader;
    }
    
    virtual void Init()
    {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);
        glClearColor(0.0f, 0.3f, 0.0f, 1.0f);
    }
    
    virtual void Draw() {}
    
    virtual void KeyDown(unsigned char const key) {}
    
    virtual void MouseDown(int const button, int const x, int const y) {}
    
    Shader* CreateDefaultShaderProgram() const
    {
        auto vshCode =
#ifdef EMSCRIPTEN
        ""
#else
        "#version 120"
#endif
        R"(
        attribute vec4 position;
        attribute vec3 normal;
        varying vec4 v_position;
        varying vec3 v_normal;
        uniform mat4 modelMat;
        uniform mat4 viewProjMat;
        
        void main()
        {
            v_position = modelMat * position;
            v_normal = mat3(modelMat) * normal;
            gl_Position = viewProjMat * v_position;
        }
        )";
        
        auto fshCode =
#ifdef EMSCRIPTEN
        "precision mediump float;"
#else
        ""
#endif
        R"(
        varying vec4 v_position;
        varying vec3 v_normal;
        uniform vec4 boxColor;
        uniform vec4 curBoxColor;
        uniform vec3 curBoxPos;
        
        void main()
        {
            vec3 l = curBoxPos - v_position.xyz;
            float diffuse = max(0.0, dot(normalize(v_normal), normalize(l))) * 0.5 + 0.5;
            vec4 lp = (normalize(curBoxColor) * 1.5 + 2.5) * diffuse / pow(length(l), 0.5);
            vec4 color = boxColor * lp;
            
            gl_FragColor = color;
        }
        )";
        
        return new Shader(vshCode, fshCode);
    }
    
    vec4 ColorFromInt(int const code) const
    {
        int a = (code & 0xFF000000) >> 24;
        int r = (code & 0x00FF0000) >> 16;
        int g = (code & 0x0000FF00) >> 8;
        int b = (code & 0x000000FF) >> 0;
        vec4 color(r, g, b, a);
        return color / float(0xFF);
    }
    
    void DrawString(std::string const str,
                    Shader * shader,
                    int const color,
                    vec3 const &pos = vec3(),
                    vec3 const &scale = vec3(1.0f, 1.0f, 1.0f),
                    mat4 const &baseMat = mat4(),
                    ObjectData::CenterType const centerType = ObjectData::CenterType::CENTER)
    {
        auto ms = ObjectData::GetStringMatrices(str, centerType);
        auto m = glm::translate(baseMat, pos);
        m = glm::scale(m, scale);
        for (auto &mm: ms) {
            shader->setUniform("modelMat", m * mm);
            shader->setUniform("boxColor", ColorFromInt(color));
            ObjectData::Draw();
        }
    }
    
    void DrawInteger(unsigned long const number,
                     int const digits,
                     Shader * shader,
                     int const color,
                     vec3 const &pos = vec3(),
                     vec3 const &scale = vec3(1.0f, 1.0f, 1.0f),
                     mat4 const &baseMat = mat4(),
                     ObjectData::CenterType const centerType = ObjectData::CenterType::CENTER)
    {
        std::ostringstream stream;
        stream << std::setfill('0') << std::setw(digits) << number;
        DrawString(stream.str(), shader, color, pos, scale, baseMat, centerType);
    }
};

/*
 タイトル画面用シーン
 */
struct TitleScene : Scene
{
    TitleScene()
    {}
    
    ~TitleScene() {}
    
    void Init()
    {
        Scene::Init();
        
        auto apl = shader->attributes["position"].location;
        auto anl = shader->attributes["normal"].location;
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        viewMat = glm::lookAt(vec3(0, 0, 20), vec3(0, 0, 0), vec3(0, 1, 0));
        
        glUseProgram(shader->program);
        mat4 viewProjMat = projMat * viewMat;
        shader->setUniform("viewProjMat", viewProjMat);
        shader->setUniform("curBoxColor", ColorFromInt(0xff006666));
        shader->setUniform("curBoxPos", vec3(0.0f, 6.0f, 4.0f));
        
        ObjectData::UseBackground(apl, anl);
        shader->setUniform("modelMat", mat4());
        shader->setUniform("boxColor", ColorFromInt(backgroundColor));
        ObjectData::Draw();
        
        
        ObjectData::UseBevelBox(apl, anl);
        DrawString("EMRIS", shader, 0xffff0000, vec3(0.0f, 5.0f, 1.0f), vec3(0.8f, 0.8f, 2.0f));
        
        
        ObjectData::UseBox(apl, anl);
        DrawString("PUSH ANY KEY!!", shader, 0xffffa500, vec3(0.0f, 0.0f, 1.0f), vec3(0.2f));
        if (tpsMode) {
            DrawString("TPS MODE", shader, 0xff00ffff, vec3(-2.5f, -2.0f, 0.0f), vec3(0.2f), mat4(), ObjectData::CenterType::LEFT_CENTER);
            DrawString("<: LEFT", shader, 0xff00ffff, vec3(-2.5f, -3.5f, 0.0f), vec3(0.2f), mat4(), ObjectData::CenterType::LEFT_CENTER);
            DrawString(">: RIGHT", shader, 0xff00ffff, vec3(-2.5f, -5.0f, 0.0f), vec3(0.2f), mat4(), ObjectData::CenterType::LEFT_CENTER);
            DrawString("^: PUT", shader, 0xff00ffff, vec3(-2.5f, -6.5f, 0.0f), vec3(0.2f), mat4(), ObjectData::CenterType::LEFT_CENTER);
            DrawString("v: TURN", shader, 0xff00ffff, vec3(-2.5f, -8.0f, 0.0f), vec3(0.2f), mat4(), ObjectData::CenterType::LEFT_CENTER);
        } else {
            DrawString("<: LEFT", shader, 0xff7cfc00, vec3(-2.5f, -3.5f, 0.0f), vec3(0.2f), mat4(), ObjectData::CenterType::LEFT_CENTER);
            DrawString(">: RIGHT", shader, 0xff7cfc00, vec3(-2.5f, -5.0f, 0.0f), vec3(0.2f), mat4(), ObjectData::CenterType::LEFT_CENTER);
            DrawString("^: TURN", shader, 0xff7cfc00, vec3(-2.5f, -6.5f, 0.0f), vec3(0.2f), mat4(), ObjectData::CenterType::LEFT_CENTER);
            DrawString("v: PUT", shader, 0xff7cfc00, vec3(-2.5f, -8.0f, 0.0f), vec3(0.2f), mat4(), ObjectData::CenterType::LEFT_CENTER);
        }
        
        shader->setUniform("curBoxPos", vec3(0.0f, -6.0f, 8.0f));
        DrawString("@PETA_OKECHAN", shader, 0xffff0000, vec3(0.0f, -9.5f, 2.0f), vec3(0.15f, 0.15f, 0.05f));
        
        DrawString("VER. " + version, shader, 0xffff0000, vec3(8.0f, -9.5f, 2.0f), vec3(0.15f, 0.15f, 0.05f));
        
        glFlush();
    }
    
    void Draw() {}
    
    void KeyDown(unsigned char const key)
    {
        if (key == 't' || key == 'T') {
            tpsMode = !tpsMode;
            SwitchScene<TitleScene>();
        } else {
            if (tpsMode) {
                SwitchScene<TPSGameScene>();
            } else {
                SwitchScene<GameScene>();
            }
        }
    }
    
    void MouseDown(int const button, int const x, int const y)
    {
        SwitchScene<GameScene>();
    }
};


/*
 ゲーム用のシーン
 */
struct GameScene : Scene
{
    enum {
        UP = 0, LEFT, RIGHT, DOWN
    };
    
    Tetris::Game *game = nullptr;
    
    GameScene()
    {}
    
    ~GameScene()
    {
        if (game) delete game;
    }
    
    void DrawBlock(Tetris::Block *block, mat4 const &baseMat)
    {
        for (auto &c: block->DataForCurrentDirection()) {
            mat4 m = glm::translate(baseMat,
                                         vec3(c.x, c.y, 0.0f));
            shader->setUniform("modelMat", m);
            ObjectData::Draw();
        }
    }
    
    void Init()
    {
        Scene::Init();
        
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        glUseProgram(shader->program);
        mat4 viewProjMat = projMat * viewMat;
        shader->setUniform("viewProjMat", viewProjMat);
        shader->setUniform("curBoxColor", ColorFromInt(0xff006666));
        shader->setUniform("curBoxPos", vec3(0.0f, 6.0f, 4.0f));
        
        game = new Tetris::Game();
        game->Start();
    }
    
    void Draw()
    {
        if (game->gameOver) {
            SwitchScene<GameOverScene>();
            return;
        }
        
        game->Tick();
        
        auto ColorFromType = &Tetris::Tetrimino::ColorFromType;
        auto apl = shader->attributes["position"].location;
        auto anl = shader->attributes["normal"].location;
        auto cb = game->currentBlock;
        auto nb = game->nextBlock;
        
        // テトリス内の座標から画面表示用の座標に変換する行列
        static auto t2gctMat = glm::translate(mat4(),
                                              vec3(-game->stage->width / 2.0f + 0.5f,
                                                   -game->stage->height / 2.0f + 0.5f,
                                                   0.0f));
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glUseProgram(shader->program);
        
        ObjectData::UseBevelBox(apl, anl);
        
        if (cb) {
            // 現在落下中のブロックについて
            
            // 共通で使用するUniform変数をセット
            auto color = ColorFromType(cb->type);
            shader->setUniform("curBoxColor", ColorFromInt(color));
            shader->setUniform("curBoxPos", vec3(t2gctMat * cb->coord, 2.0f));
            
            // 描画
            shader->setUniform("boxColor", ColorFromInt(color));
            auto m = glm::translate(t2gctMat, vec3(vec2(cb->coord), 0.0f));
            DrawBlock(cb, m);
        }
        
        if (nb) {
            // 次に落下するブロックを描画する
            auto color = ColorFromType(nb->type);
            shader->setUniform("boxColor", ColorFromInt(color));
            auto m = glm::translate(mat4(), vec3(8.0f, 6.0f, 1.0f));
            m = glm::scale(m, vec3(1.0f, 1.0f, 0.2f));
            DrawBlock(nb, m);
        }
        
        // ステージを描画する
        for (int x = 0; x < game->stage->width; ++x) {
            for (int y = 0; y < game->stage->height; ++y) {
                auto c = game->stage->cols[x][y];
                if (c) {
                    auto color = ColorFromType(c);
                    shader->setUniform("boxColor", ColorFromInt(color));
                    
                    auto m = glm::translate(t2gctMat, vec3(x, y, 0.0f));
                    shader->setUniform("modelMat", m);
                    ObjectData::Draw();
                }
            }
        }
        
        // 背景を描画する
        ObjectData::UseBackground(apl, anl);
        shader->setUniform("modelMat", mat4());
        shader->setUniform("boxColor", ColorFromInt(backgroundColor));
        ObjectData::Draw();
        
        if (cb) {
            // 落下予測点に半透明で描画
            ObjectData::UseBevelBox(apl, anl);
            glEnable(GL_BLEND);
            shader->setUniform("boxColor", ColorFromInt(0x55ffffff));
            auto m = glm::translate(t2gctMat, vec3(vec2(game->iip), 0.0f));
            shader->setUniform("modelMat", m);
            DrawBlock(cb, m);
            glDisable(GL_BLEND);
        }
        
        // スコアなどを表示する
        ObjectData::UseBox(apl, anl);
        DrawString("NEXT", shader, 0xffffa533, vec3(8.0f, 8.0f, 1.0f), vec3(0.2f));
        
        DrawString("SCORE", shader, 0xffffa533, vec3(8.0f, 0.0f, 1.0f), vec3(0.2f));
        DrawInteger(game->score, 6, shader, 0xffff3333, vec3(8.0f, -1.5f, 1.0f), vec3(0.2f));
        
        glFlush();
    }
    
    void KeyDown(unsigned char const key)
    {
        switch (key) {
            case 'w':
                game->Operation(Tetris::Game::Ops::TURN);
                break;
            case 'a':
                game->Operation(Tetris::Game::Ops::MOVE_L);
                break;
            case 'd':
                game->Operation(Tetris::Game::Ops::MOVE_R);
                break;
            case 's':
                game->Operation(Tetris::Game::Ops::DOWN);
                break;
            case 'p':
                if (game->paused) {
                    game->Resume(true);
                } else {
                    game->Pause();
                }
                break;
        }
    }
    
    void MouseDown(int const button, int const x, int const y)
    {
        
    }
};


/*
 TPSモード時のゲーム用シーン
 */
struct TPSGameScene : GameScene
{
    template<typename T, typename U>
    struct Tween
    {
        bool live;
        T value;
        T origin;
        T target;
        U duration;
        U now;
        std::function<U(U)> ease;
        std::function<void(T)> onUpdate;
        std::function<void(void)> onComplete;
        
        Tween()
        : live(false)
        {}
        
        void Start(T _value, T _target, U _duration,
              std::function<U(U)>  _ease,
              std::function<void(T)> _onUpdate,
              std::function<void(void)>  _onComplete)
        {
            live = true, value = _value, origin = _value, target = _target;
            duration = _duration, now = 0.0, ease = _ease;
            onUpdate = _onUpdate, onComplete = _onComplete;
        }
        
        void Update()
        {
            if (!live) return;
            now += U(1.0);
            U t = ease(now / duration);
            value = origin * (U(1.0) - t) + target * t;
            onUpdate(value);
            if (now >= duration) {
                live = false;
                onComplete();
            }
        }
    };
    
    Tween<vec2, float> tween;
    
    TPSGameScene()
    : GameScene(), tween()
    {}
    
    void Draw()
    {
        /*
         現在のブロックの移動についてアニメーションを行う。
         ゲーム内の時間進行とアニメーションの進行は交互に行う。
         */
        const int maxAnimFrame = 3;
        static vec2 coordForDraw = game->InitialCoord();
        
        if (game->gameOver) {
            SwitchScene<GameOverScene>();
            return;
        }
        
        game->Tick();
        
        auto ColorFromType = &Tetris::Tetrimino::ColorFromType;
        auto apl = shader->attributes["position"].location;
        auto anl = shader->attributes["normal"].location;
        auto cb = game->currentBlock;
        auto nb = game->nextBlock;
        
        // テトリス内の座標から画面表示用の座標に変換する行列
        static auto t2gctMat = glm::translate(mat4(),
                                              vec3(-game->stage->width / 2.0f + 0.5f,
                                                   -game->stage->height / 2.0f + 0.5f,
                                                   0.0f));
        
        
        if (!tween.live && cb && vec2(cb->coord) != coordForDraw) {
            // 非アニメーション中に現在のブロックの座標が変わったら
            // アニメーションを開始する
            tween.Start(coordForDraw, vec2(cb->coord), maxAnimFrame, [](float t){
                // ease
                return t;
            }, [](vec2 v){
                // onUpdate
                coordForDraw = v;
            }, [&](){
                // onComplete
                game->Resume();
            });
            game->Pause();
        }
        tween.Update();
        
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glUseProgram(shader->program);
        
        ObjectData::UseBevelBox(apl, anl);
        
        if (cb) {
            // 現在落下中のブロックについて
            
            // その位置を元にビュー行列を作成しセットする
            auto p = t2gctMat * coordForDraw;
            auto eye = vec3(p.x, p.y + 3.0f, 3.0f);
            auto center = eye + vec3(0.0f, -4.5f, -3.0f);
            viewMat = glm::lookAt(eye, center, vec3(0.0f, 0.0f, 1.0f));
            shader->setUniform("viewProjMat", projMat * viewMat);
            
            // 共通で使用するUniform変数をセット
            auto color = ColorFromType(cb->type);
            shader->setUniform("curBoxColor", ColorFromInt(color));
            shader->setUniform("curBoxPos", vec3(t2gctMat * coordForDraw, 2.0f));
            
            // 描画
            shader->setUniform("boxColor", ColorFromInt(color));
            auto m = glm::translate(t2gctMat, vec3(coordForDraw, 0.0f));
            DrawBlock(cb, m);
            

        }
        
        // ステージを描画する
        for (int x = 0; x < game->stage->width; ++x) {
            for (int y = 0; y < game->stage->height; ++y) {
                auto c = game->stage->cols[x][y];
                if (c) {
                    auto color = ColorFromType(c);
                    shader->setUniform("boxColor", ColorFromInt(color));
                    
                    auto m = glm::translate(t2gctMat, vec3(x, y, 0.0f));
                    shader->setUniform("modelMat", m);
                    ObjectData::Draw();
                }
            }
        }
        
        // 背景を描画する
        ObjectData::UseBackground(apl, anl);
        shader->setUniform("modelMat", mat4());
        shader->setUniform("boxColor", ColorFromInt(backgroundColor));
        ObjectData::Draw();
        
        // 落下予測点に半透明で描画
        if (cb){
            ObjectData::UseBevelBox(apl, anl);
            glEnable(GL_BLEND);
            shader->setUniform("boxColor", ColorFromInt(0x55ffffff));
            auto m = glm::translate(t2gctMat, vec3(vec2(game->iip), 0.0f));
            shader->setUniform("modelMat", m);
            DrawBlock(cb, m);
            glDisable(GL_BLEND);
        }
    
        auto viewMatInv = glm::inverse(viewMat);
        glClear(GL_DEPTH_BUFFER_BIT);
        if (nb) {
            // 次に落下するブロックを描画する
            ObjectData::UseBevelBox(apl, anl);
            auto color = ColorFromType(nb->type);
            shader->setUniform("boxColor", ColorFromInt(color));
            auto m = glm::translate(viewMatInv, vec3(0.4f, 0.3f, -1.0f));
            m = glm::scale(m, vec3(0.05f, 0.05f, 0.01f));
            DrawBlock(nb, m);
        }
        
        // スコアなどを表示する
        ObjectData::UseBox(apl, anl);
        DrawString("NEXT", shader, 0xffffa533, vec3(8.0f, 8.0f, -20.0f), vec3(0.2f), viewMatInv);
        
        DrawString("SCORE", shader, 0xffffa533, vec3(8.0f, 0.0f, -20.0f), vec3(0.2f), viewMatInv);
        DrawInteger(game->score, 6, shader, 0xffff3333, vec3(8.0f, -1.5f, -20.0f), vec3(0.2f), viewMatInv);
        
        glFlush();
    }
    
    void KeyDown(unsigned char const key)
    {
        switch (key) {
            case 'w':
                game->Operation(Tetris::Game::Ops::DOWN);
                break;
            case 'a':
                game->Operation(Tetris::Game::Ops::MOVE_R);
                break;
            case 'd':
                game->Operation(Tetris::Game::Ops::MOVE_L);
                break;
            case 's':
                game->Operation(Tetris::Game::Ops::TURN);
                break;
            case 'p':
                if (game->paused) {
                    game->Resume(true);
                } else {
                    game->Pause();
                }
                break;
        }
    }
};


/*
 ゲームオーバー画面用のシーン
 */
struct GameOverScene : Scene
{
    GameOverScene()
    {}
    
    ~GameOverScene() {}
    
    void Init()
    {
        Scene::Init();
        
        auto apl = shader->attributes["position"].location;
        auto anl = shader->attributes["normal"].location;
        
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glClear(GL_DEPTH_BUFFER_BIT);
        
        glUseProgram(shader->program);
        mat4 viewProjMat = projMat * viewMat;
        shader->setUniform("viewProjMat", viewProjMat);
        shader->setUniform("curBoxColor", ColorFromInt(0xffffffff));
        
        ObjectData::UseBox(apl, anl);
        
        glEnable(GL_BLEND);
        auto m = glm::translate(mat4(), vec3(0.0f, 0.0f, 0.0f));
        m = glm::scale(m, vec3(40.0f, 40.0f, 1.5f));
        shader->setUniform("modelMat", m);
        shader->setUniform("boxColor", ColorFromInt(0x9f000000));
        shader->setUniform("curBoxPos", vec3(0.0f, 0.0f, 40.0f));
        ObjectData::Draw();
        glDisable(GL_BLEND);
        
        shader->setUniform("curBoxPos", vec3(0.0f, 0.0f, 4.0f));
        DrawString("GAMEOVER", shader, 0xffff3333, vec3(0.0f, 3.0f, 2.0f), vec3(0.5f));
        DrawString("R: RESTART", shader, 0xff7cfc00, vec3(0.0f, -3.5f, 2.0f), vec3(0.2f));
        DrawString("T: ???????", shader, 0xff7cfc00, vec3(0.0f, -6.0f, 2.0f), vec3(0.2f));
        
        glFlush();
    }
    
    void Draw() {}
    
    void KeyDown(unsigned char const key)
    {
        if (key == 'r' || key == 'R') {
            tpsMode = false;
            SwitchScene<TitleScene>();
        }
        if (key == 't' || key == 'T') {
            tpsMode = true;
            SwitchScene<TitleScene>();
        }
    }
};


/*
 シーンを切り替える
 各シーン内部から呼び出される
 */
template<typename T>
void SwitchScene()
{
    if (currentScene) delete currentScene;
    currentScene = new T();
    currentScene->Init();
}


/*
 指定のfpsで画面を更新するための
 glutTimerFunc用のコールバック
 */
void timercb(int value) {
    glutPostRedisplay();
    glutTimerFunc(1000 / fps, timercb, 0);
};


int main(int argc, char * argv[])
{
    glutInitWindowSize(screenWidth, screenHeight);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH);
    glutCreateWindow(argv[0]);
	glutDisplayFunc([](){
        currentScene->Draw();
    });
    glutKeyboardFunc([](unsigned char keychar, int x, int y){
        currentScene->KeyDown(keychar);
    });
    glutSpecialFunc([](int key, int x, int y){
        unsigned char keychar = 0;
        switch (key) {
            case GLUT_KEY_UP: keychar = 'w'; break;
            case GLUT_KEY_LEFT: keychar = 'a'; break;
            case GLUT_KEY_RIGHT: keychar = 'd'; break;
            case GLUT_KEY_DOWN: keychar = 's'; break;
        }
        currentScene->KeyDown(keychar);
    });
    glutMouseFunc([](int button, int state, int x, int y){
        if (state == GLUT_DOWN) currentScene->MouseDown(button, x, y);
    });
    glutTimerFunc(1000 / fps, timercb, 0);
    
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL version: " <<  glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    
    SwitchScene<TitleScene>();
    
	glutMainLoop();
    
    return 0;
}

