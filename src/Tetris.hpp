//
//  Tetris.hpp
//  emris
//
//  Created by peta on 2013/11/19.
//  Copyright (c) 2013 peta.okechan.net. All rights reserved.
//

#ifndef MacTetris_Tetris_hpp
#define MacTetris_Tetris_hpp

#include <iostream>
#include <vector>
#include <queue>
#include <random>
#include <functional>
#include <sys/time.h>

namespace Tetris {
    
    struct Tetrimino
    {
        using Cell = unsigned char;
        
        enum Type : Cell
        {
            I = 1, O, S, Z, J, L, T,
            SIZE = 7,
            NONE = 0
        };
        
        static Type GetRandomType()
        {
            static std::mt19937 engine;
            static std::uniform_int_distribution<Type> distribution(Type::I, Type::T);
            return distribution(engine);
        }
        
        static unsigned int ColorFromType(Type const type)
        {
            switch (type) {          // ARGB
                case Type::I: return 0xFF00FFFF;  // 水色
                case Type::O: return 0xFFFFFF00;  // 黄色
                case Type::S: return 0xFF7CFC00;  // 黄緑
                case Type::Z: return 0xFFFF3333;  // 赤
                case Type::J: return 0xFF6666FF;  // 青
                case Type::L: return 0xFFFFA500;  // オレンジ
                case Type::T: return 0xFF9400D3;  // 紫
                default: throw "invalid type.";
            }
        }
        
        struct Coord
        {
            int x, y;
            Coord(int const x, int const y)
            : x(x), y(y)
            {}
            
            Coord operator + (Coord const & c) const
            {
                Coord tmp(this->x + c.x, this->y + c.y);
                return tmp;
            }
            
            Coord & operator += (Coord const & c)
            {
                this->x += c.x, this->y += c.y;
                return *this;
            }
            
            bool operator == (Coord const & c) const
            {
                return (this->x == c.x && this->y == c.y);
            }
            
            bool operator != (Coord const & c) const
            {
                return !(*this == c);
            }
        };
        
        using BlockData = std::vector<Coord>;
        using BlockDataSet = std::vector<BlockData>;
        
        static BlockDataSet BlockDataSetFromType(Type const type)
        {
            BlockDataSet bds;
            switch (type) {
                case Type::I:
                    bds.push_back({{-2, 0}, {-1, 0}, {0, 0}, {1, 0}});
                    bds.push_back({{0, -2}, {0, -1}, {0, 0}, {0, 1}});
                    break;
                case Type::O:
                    bds.push_back({{0, 0}, {1, 0}, {0, 1}, {1, 1}});
                    break;
                case Type::S:
                    bds.push_back({{0, 0}, {1, 0}, {-1, -1}, {0, -1}});
                    bds.push_back({{-1, 1}, {-1, 0}, {0, 0}, {0, -1}});
                    break;
                case Type::Z:
                    bds.push_back({{-1, 0}, {0, 0}, {0, -1}, {1, -1}});
                    bds.push_back({{0, -1}, {0, 0}, {1, 0}, {1, 1}});
                    break;
                case Type::J:
                    bds.push_back({{-1, 0}, {0, 0}, {1, 0}, {1, -1}});
                    bds.push_back({{0, -1}, {0, 0}, {0, 1}, {1, 1}});
                    bds.push_back({{-1, -1}, {0, -1}, {1, -1}, {-1, 0}});
                    bds.push_back({{0, -1}, {0, 0}, {0, 1}, {-1, -1}});
                    break;
                case Type::L:
                    bds.push_back({{-1, 0}, {0, 0}, {1, 0}, {-1, -1}});
                    bds.push_back({{0, -1}, {0, 0}, {0, 1}, {1, -1}});
                    bds.push_back({{-1, -1}, {0, -1}, {1, -1}, {1, 0}});
                    bds.push_back({{0, -1}, {0, 0}, {0, 1}, {-1, 01}});
                    break;
                case Type::T:
                    bds.push_back({{-1, 0}, {0, 0}, {1, 0}, {0, -1}});
                    bds.push_back({{0, 1}, {0, 0}, {1, 0}, {0, -1}});
                    bds.push_back({{-1, -1}, {0, -1}, {1, -1}, {0, 0}});
                    bds.push_back({{-1, 0}, {0, 0}, {0, 1}, {0, -1}});
                    break;
                default: throw "invarid type.";
            }
            return std::move(bds);
        }
    };
    
    struct Block
    {
        Tetrimino::Type type;
        Tetrimino::BlockDataSet dataSet;
        Tetrimino::Coord coord;
        unsigned long direction;
        
        Block(Tetrimino::Type const type, Tetrimino::Coord const coord)
        : type(type), dataSet(Tetrimino::BlockDataSetFromType(type)), coord(coord), direction(0)
        {}
        
        Block(Tetrimino::Coord const coord)
        : Block(Tetrimino::GetRandomType(), coord)
        {}
        
        void Turn(int const turnCount = 1)
        {
            direction = (direction + turnCount) % dataSet.size();
        }
        
        Tetrimino::BlockData & DataForCurrentDirection(int const dirOffset = 0)
        {
            return dataSet[(direction + dirOffset) % dataSet.size()];
        }
    };
    
    struct Stage
    {
        using Col = std::vector<Tetrimino::Type>;
        
        int width;
        int height;
        std::vector<Col> cols;
        
        Stage(int const width, int const height)
        : width(width), height(height), cols(width)
        {
            for (auto &col: cols) {
                col = Col(height);
            }
        }
        
        Col & operator [] (unsigned long const i)
        {
            return cols[i];
        }
        
        bool Existable(Block &block, Tetrimino::Coord const &offset = {0, 0}, int const dirOffset = 0)
        {
            for (auto &c: block.DataForCurrentDirection(dirOffset)) {
                auto p = c + block.coord + offset;
                if (p.y < height)
                    if (p.x < 0 || p.x >= width || p.y < 0 || cols[p.x][p.y] > 0)
                        return false;
            }
            return true;
        }
        
        void Bake(Block &block)
        {
            for (auto &c: block.DataForCurrentDirection()) {
                auto p = c + block.coord;
                cols[p.x][p.y] = block.type;
            }
        }
        
        unsigned long LinesErased()
        {
            std::vector<int> linesErased;
            for (int y = height - 1; y >= 0; --y) {
                bool hasLine = true;
                for (int x = 0; x < width; ++x) {
                    if (cols[x][y] == Tetrimino::Type::NONE) {
                        hasLine = false;
                        break;
                    }
                }
                if (hasLine) linesErased.push_back(y);
            }
            
            for (auto ey: linesErased) {
                for (int y = ey; y < height - 1; ++y) {
                    for (int x = 0; x < width; ++x) {
                        cols[x][y] = cols[x][y+1];
                    }
                }
                for (int x = 0; x < width; ++x) {
                    cols[x][height-1] = Tetrimino::Type::NONE;
                }
            }
            
            return linesErased.size();
        }
    };
    
    struct Game
    {
        
        enum struct Ops
        {
            MOVE_L,
            MOVE_R,
            TURN,
            DOWN
        };
        
        Stage *stage = nullptr;
        Block *currentBlock = nullptr;
        Block *nextBlock = nullptr;
        Tetrimino::Coord iip;   // Instantaneous impact point
        bool gameOver;
        unsigned long linesErased;
        unsigned long score;
        std::function<double (unsigned long)> timingDicider;
        double past;
        double now;
        double pauseTime;
        bool paused;
        double timing;
        std::queue<Ops> opsQueue;
        
        Game(int const width = 10, int const height = 20)
        : iip(0, 0), gameOver(false), linesErased(0), score(0), past(0.0), now(0.0), pauseTime(0.0), paused(false), opsQueue()
        {
            stage = new Stage(width, height);
            timingDicider = [](unsigned long score) -> double {
                return std::max(exp(-double(score) / 500.0), 0.01) / 2.0;
            };
            timing = timingDicider(score);
        }
        
        ~Game()
        {
            if (stage) delete stage;
            if (currentBlock) delete currentBlock;
            if (nextBlock) delete nextBlock;
        }
        
        Tetrimino::Coord InitialCoord() const
        {
            return {stage->width / 2, stage->height - 1};
        }
        
        double GetTime() const
        {
            timeval tvnow;
            gettimeofday(&tvnow, nullptr);
            return tvnow.tv_sec + (tvnow.tv_usec * 1e-6);
        }
        
        void CalcIIP()
        {
            Tetrimino::Coord offset = {0, 0};
            while (stage->Existable(*currentBlock, offset)) offset += {0, -1};
            iip = currentBlock->coord + offset;
            ++iip.y;
        }
        
        void Start()
        {
            past = 0.0;
            paused = false;
            opsQueue = std::queue<Ops>();
            nextBlock = new Block(InitialCoord());
        }
        
        double TickProgression() const
        {
            return (timing > 0.0)?((now - past) / timing):0.0;
        }
        
        void Pause()
        {
            pauseTime = GetTime();
            paused = true;
        }
        
        void Resume(bool const clearOpsQueue = false)
        {
            if (clearOpsQueue) opsQueue = std::queue<Ops>();
            paused = false;
            past = GetTime() - (pauseTime - past);
        }
        
        void Tick()
        {
            if (paused) return;
            
            ProcessOpsQueue();
            
            now = GetTime();
            if (now - past < timing) {
                return;
            }
            
            if (!currentBlock) {
                currentBlock = nextBlock;
                nextBlock = new Block(InitialCoord());
                if (stage->Existable(*currentBlock)) {
                    CalcIIP();
                } else {
                    gameOver = true;
                }
            } else {
                // 一段下ろせるならおろす
                // おろせないなら確定
                if (stage->Existable(*currentBlock, {0, -1})) {
                    currentBlock->coord += {0, -1};
                } else {
                    stage->Bake(*currentBlock);
                    delete currentBlock;
                    currentBlock = nullptr;
                    // 確定したらスキャン
                    auto le = stage->LinesErased();
                    linesErased += le;
                    for (int i = 0; i < le; ++i) score += 10 * (i + 1);
                    timing = timingDicider(score);
                }
            }
            
            past = GetTime();
        }
        
        void ProcessOpsQueue()
        {
            for (int i = 0; i < opsQueue.size(); ++i) {
                auto op = opsQueue.front();
                opsQueue.pop();
                if (currentBlock) {
                    switch (op) {
                        case Ops::MOVE_L:
                            if (stage->Existable(*currentBlock, {-1, 0}))
                                currentBlock->coord += {-1, 0};
                            CalcIIP();
                            break;
                        case Ops::MOVE_R:
                            if (stage->Existable(*currentBlock, {1, 0}))
                                currentBlock->coord += {1, 0};
                            CalcIIP();
                            break;
                        case Ops::TURN:
                            if (stage->Existable(*currentBlock, {0, 0}, 1))
                                currentBlock->Turn();
                            CalcIIP();
                            break;
                        case Ops::DOWN:
                            //CalcIIP();
                            if (iip != currentBlock->coord) {
                                currentBlock->coord = iip;
                                past = GetTime();
                            }
                            break;
                    }
                }
            }
        }
        
        void Operation(Ops const op)
        {
            opsQueue.push(op);
        }
    };
}

#endif
