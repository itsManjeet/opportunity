#include <opportunity.hh>
#include <sys/mount.h>

using namespace opportunity;

#define LOCK_GUARD(x) std::lock_guard<std::mutex> lock((x))

void
util::installer::get_data(double *fc, std::string* mesg) const
{
    std::lock_guard<std::mutex> lock(__mutex);
    if (fc) *fc = __progress;
    if (mesg) *mesg = __mesg;
}

void
util::installer::stop()
{
    std::lock_guard<std::mutex> lock(__mutex);
    __stop = true;
}

void
util::installer::start(window* caller, data* __install_data)
{
    {
        std::lock_guard<std::mutex> lock(__mutex);
        __progress = 0.0;
        __mesg = "initalizing";
    }

  
    {
        LOCK_GUARD(__mutex);
        __progress = 0.2;
        __mesg = io::sprint(__progress * 100, "% preparing disk");
    }

    caller->notify();

    int ret = system(io::sprint("mkfs.ext4 -F ", __install_data->root_disk).c_str());
    if (WEXITSTATUS(ret)) {
        {
            LOCK_GUARD(__mutex);
            __mesg = io::sprint("failed to prepare disk ", __install_data->root_disk);
            io::error(__mesg);
            __stop = true;
            __error = true;
        }
        caller->notify();
        return;
    }

    caller->notify();

    fs::makedir(__WORK_DIR__);
    ret = mount(io::sprint(__install_data->root_disk).c_str(), __WORK_DIR__ ,"ext4",0, "");
    if (ret) {
        {
            LOCK_GUARD(__mutex);
            __mesg = io::sprint("failed to mount disk ", __install_data->root_disk);
            io::error(__mesg);
            __stop = true;
            __error = true;
        }
        caller->notify();
        
        return;
    }
    

        caller->notify();

    {
        LOCK_GUARD(__mutex);
        __progress = 0.5;
        __mesg = io::sprint(__progress * 100, "% install system into ",__install_data->root_disk);

    }
        
    caller->notify();

    ret = system(io::sprint("unsquashfs -f -d ", __WORK_DIR__ , " ", __SQUA_IMAGE__ ).c_str());
    if (WEXITSTATUS(ret)) {
        {
            LOCK_GUARD(__mutex);
            __mesg = io::sprint("failed to install ", __install_data->root_disk, " ", __SQUA_IMAGE__);
            io::error(__mesg);
            __stop = true;
            __error = true;
        }
        caller->notify();
        return;
    }

    
    {
        LOCK_GUARD(__mutex);
        __progress = 0.8;
        __mesg = io::sprint(__progress * 100, "% install bootloader ",__install_data->root_disk);
        __error = true;

    }
    

    caller->notify();

    if (__install_data->efi)
        ret = system(io::sprint("releax-chroot ",__WORK_DIR__, " EFI ", __install_data->efi_disk).c_str());
    else
        ret = system(io::sprint("releax-chroot ", __WORK_DIR__, " LEGACY ", __install_data->root_disk).c_str());
    
    if (WEXITSTATUS(ret)) {
        {
            LOCK_GUARD(__mutex);
            __mesg = io::sprint("failed to install bootloader ", __install_data->efi);
            io::error(__mesg);
            __stop = true;
            __error = true;
        }
        caller->notify();
    }

    caller->notify();
    
    
    ret = system(io::sprint("umount ", __WORK_DIR__).c_str());
    if (WEXITSTATUS(ret)) {
        {
            LOCK_GUARD(__mutex);
            __mesg = io::sprint("failed to cleanup system");
            io::error(__mesg);
            __stop = true;
            __error = true;
        }
    }

    caller->notify();

    {
        LOCK_GUARD(__mutex);
        __error = false;
    }
}