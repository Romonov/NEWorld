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

#include "worldclient.h"
#include "gameconnection.h"

void WorldClient::renderUpdate(const Vec3i& position)
{
    Vec3i chunkpos = getChunkPos(position);
    for (auto&& chunk : mChunks)
    {
        // In render range, pending to render
        if (chunkpos.chebyshevDistance(chunk->getPosition()) <= mRenderDist)
        {
            if ((mChunkRenderers.find(chunk.get()) == mChunkRenderers.end()) &&
                    neighbourChunkLoadCheck(chunk->getPosition()))
            {
                // Get chunk center pos
                Vec3i curPos = chunk->getPosition() * Chunk::Size() + middleOffset();
                // Distance from center pos
                mChunkRenderList.insert((curPos - position).lengthSqr(), chunk.get());
            }
        }
        else
        {
            auto iter = mChunkRenderers.find(chunk.get());
            if (iter != mChunkRenderers.end())
                mChunkRenderers.erase(iter);
        }
    }

    for (auto&& op : mChunkRenderList)
    {
        op.second->setUpdated(false);
        mChunkRenderers.insert(
                std::pair<Chunk*, ChunkRenderer>(op.second, std::move(ChunkRenderer(op.second))));
    }
    mChunkRenderList.clear();
}

size_t WorldClient::render(const Vec3i& position) const
{
    Vec3i chunkpos = getChunkPos(position);
    size_t renderedChunks = 0;
    for (auto&& c : mChunkRenderers)
    {
        if (chunkpos.chebyshevDistance(c.first->getPosition()) <= mRenderDist)
        {
            Renderer::translate(Vec3f(c.first->getPosition() * Chunk::Size()));
            c.second.render();
            Renderer::translate(Vec3f(-c.first->getPosition() * Chunk::Size()));
            ++renderedChunks;
        }
    }
    return renderedChunks;
}

void WorldClient::sortChunkLoadUnloadList(const Vec3i& centerPos)
{
    // centerPos to chunk coords
    Vec3i centerCPos = getChunkPos(centerPos);

    for (size_t ci = 0; ci < getChunkCount(); ci++)
    {
        Vec3i curPos = getChunk(ci).getPosition();
        // Out of load range, pending to unload
        if (centerCPos.chebyshevDistance(curPos) > mLoadRange)
        {
            // Get chunk center pos
            curPos = curPos * Chunk::Size() + middleOffset();
            // Distance from center
            mChunkUnloadList.insert((curPos - centerPos).lengthSqr(), &getChunk(ci));
        }
    }

    for (int x = centerCPos.x - mLoadRange; x <= centerCPos.x + mLoadRange; x++)
        for (int y = centerCPos.y - mLoadRange; y <= centerCPos.y + mLoadRange; y++)
            for (int z = centerCPos.z - mLoadRange; z <= centerCPos.z + mLoadRange; z++)
                // In load range, pending to load
                if (!isChunkLoaded(Vec3i(x, y, z)))
                {
                    Vec3i curPos = Vec3i(x, y, z) * Chunk::Size() + middleOffset();
                    // Distance from centerPos
                    mChunkLoadList.insert((curPos - centerPos).lengthSqr(), Vec3i(x, y, z));
                }
}

void WorldClient::tryLoadChunks(GameConnection& conn)
{
    for (auto&& op : mChunkLoadList)
        conn.getChunk(op.second);
    for (auto&& op : mChunkUnloadList)
        deleteChunk(op.second->getPosition());
    mChunkLoadList.clear();
    mChunkUnloadList.clear();
}

bool WorldClient::neighbourChunkLoadCheck(const Vec3i& pos)
{
    constexpr std::array<Vec3i, 6> delta
    {
            Vec3i(1, 0, 0), Vec3i(-1, 0, 0), Vec3i(0, 1, 0),
            Vec3i(0,-1, 0), Vec3i(0, 0, 1), Vec3i(0, 0,-1)
    };
    for (auto&& p : delta)
        if (!isChunkLoaded(pos + p))
            return false;
    return true;
}
