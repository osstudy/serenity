#pragma once

#include <VirtualFileSystem/CharacterDevice.h>
#include "DoubleBuffer.h"

class SlavePTY;

class MasterPTY final : public CharacterDevice {
public:
    explicit MasterPTY(unsigned index);
    virtual ~MasterPTY() override;
    void set_slave(SlavePTY& slave) { m_slave = &slave; }

    virtual ssize_t read(Process&, byte*, size_t) override;
    virtual ssize_t write(Process&, const byte*, size_t) override;
    virtual bool can_read(Process&) const override;
    virtual bool can_write(Process&) const override;
    virtual bool is_master_pty() const override { return true; }

    String pts_name() const;
    void on_slave_write(const byte*, size_t);

private:
    unsigned m_index;
    SlavePTY* m_slave { nullptr };
    DoubleBuffer m_buffer;
};