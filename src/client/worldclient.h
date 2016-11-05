/*
* NEWorld: A free game with similar rules to Minecraft.
* Copyright (C) 2016 NEWorld Team
*
* This file is part of NEWorld.
* NEWorld is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* NEWorld is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with NEWorld.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef WORLDCLIENT_H_
#define WORLDCLIENT_H_

#include <world.h>
#include "chunkclient.h"
class GameConnection;

const int MaxChunkRenderCount = 4;
constexpr int MaxChunkLoadCount = 64, MaxChunkUnloadCount = 64;
class WorldClient : public World
{
public:
    WorldClient(const std::string& name, const PluginManager& plugins, const BlockManager& blocks)
        : World(name, plugins, blocks)
    {
    }

    WorldClient(World&& world) : World(std::move(world))
    {
    }

    Chunk* addChunk(const Vec3i& chunkPos) override;

    void setRenderDistance(int x)
    {
        mRenderDist = x;
    }

    // Build/Destroy VBO
    void renderUpdate(const Vec3i& position);

    // Render all chunks
    size_t render(const Vec3i& position) const;
    // Set load range
    void setLoadRange(int x);

    // Find the nearest chunks in load range to load, fartherest chunks out of load range to unload
    void sortChunkLoadUnloadList(const Vec3i& centerPos);

    void tryLoadChunks(GameConnection& conn);
private:
    // Render distance
    int mRenderDist = 0;
    // Render build list
    std::pair<ChunkClient*, int> mChunkRenderList[MaxChunkRenderCount];

    int mChunkLoadCount, mChunkUnloadCount = 0, mLoadRange = 0;
    // Chunk load list [position, distance]
    std::pair<Vec3i, int> mChunkLoadList[MaxChunkLoadCount];
    // Chunk unload list [pointer, distance]
    std::pair<Chunk*, int> mChunkUnloadList[MaxChunkUnloadCount];
};

#endif // !WORLDCLIENT_H_
