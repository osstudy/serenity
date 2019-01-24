#include "DiskBackedFileSystem.h"
#include "i386.h"

//#define DBFS_DEBUG

DiskBackedFS::DiskBackedFS(RetainPtr<DiskDevice>&& device)
    : m_device(move(device))
{
    ASSERT(m_device);
}

DiskBackedFS::~DiskBackedFS()
{
}

bool DiskBackedFS::writeBlock(unsigned index, const ByteBuffer& data)
{
#ifdef DBFS_DEBUG
    kprintf("DiskBackedFileSystem::writeBlock %u, size=%u\n", index, data.size());
#endif
    ASSERT(data.size() == blockSize());
    DiskOffset baseOffset = static_cast<DiskOffset>(index) * static_cast<DiskOffset>(blockSize());
    return device().write(baseOffset, blockSize(), data.pointer());
}

bool DiskBackedFS::writeBlocks(unsigned index, unsigned count, const ByteBuffer& data)
{
#ifdef DBFS_DEBUG
    kprintf("DiskBackedFileSystem::writeBlocks %u x%u\n", index, count);
#endif
    DiskOffset baseOffset = static_cast<DiskOffset>(index) * static_cast<DiskOffset>(blockSize());
    return device().write(baseOffset, count * blockSize(), data.pointer());
}

ByteBuffer DiskBackedFS::readBlock(unsigned index) const
{
#ifdef DBFS_DEBUG
    kprintf("DiskBackedFileSystem::readBlock %u\n", index);
#endif
    auto buffer = ByteBuffer::create_uninitialized(blockSize());
    //kprintf("created block buffer with size %u\n", blockSize());
    DiskOffset baseOffset = static_cast<DiskOffset>(index) * static_cast<DiskOffset>(blockSize());
    auto* bufferPointer = buffer.pointer();
    bool success = device().read(baseOffset, blockSize(), bufferPointer);
    ASSERT(success);
    ASSERT(buffer.size() == blockSize());
    return buffer;
}

ByteBuffer DiskBackedFS::readBlocks(unsigned index, unsigned count) const
{
    if (!count)
        return nullptr;
    if (count == 1)
        return readBlock(index);
    auto blocks = ByteBuffer::create_uninitialized(count * blockSize());
    byte* out = blocks.pointer();

    for (unsigned i = 0; i < count; ++i) {
        auto block = readBlock(index + i);
        if (!block)
            return nullptr;
        memcpy(out, block.pointer(), block.size());
        out += blockSize();
    }

    return blocks;
}

void DiskBackedFS::setBlockSize(unsigned blockSize)
{
    if (blockSize == m_blockSize)
        return;
    m_blockSize = blockSize;
}