#include <opportunity.hh>
#include <sys/mount.h>
#include <chrono>
#include <future>


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
util::installer::run(data* __install_data, window* caller)
{
    {
        LOCK_GUARD(__mutex);
        __progress = 0.1;
        __mesg = "Initializing Installer";
    }

    io::info("Formating System");
    int ret = WEXITSTATUS(system(io::sprint("/sbin/mkfs.ext4 -F ", __install_data->root_disk).c_str()));
    if (ret) {
        {
            io::info("Error while formating system");
            LOCK_GUARD(__mutex);
            __mesg = "Failed to format root device";
            __error = true;
            __stop = true;
        }
        return;
    }

    {
        LOCK_GUARD(__mutex);
        __progress = 0.2;
        __mesg = "preparing work dir";
    }

    fs::makedir(__WORK_DIR__);
    io::info("mounting device");

    ret = WEXITSTATUS(system(io::sprint("mount ",__install_data->root_disk, " ", __WORK_DIR__).c_str()));
    if (ret) {
        {
            io::info("Error while extract system");
            LOCK_GUARD(__mutex);
            __mesg = "Failed to extract system";
            __error = true;
            __stop = true;
        }
        return;
    }

    {
        LOCK_GUARD(__mutex);
        __progress = 0.5;
        __mesg = "Extracting system into roots [may take upto 20min]";
    }

    io::info("unsquashing image");
    ret = WEXITSTATUS(system(io::sprint("unsquashfs -f -d ",__WORK_DIR__, " " , __SQUA_IMAGE__).c_str()));
    if (ret) {
        {
            io::info("Error while extract system");
            LOCK_GUARD(__mutex);
            __mesg = "Failed to extract system";
            __error = true;
            __stop = true;
        }
        return;
    }

    {
        LOCK_GUARD(__mutex);
        __progress = 0.8;
        __mesg = "Installing Bootloader";
    }

    io::info("installing bootloader");
    std::string __bootloader_type = "LEGACY";
    std::string __ddir = __install_data->root_disk;
    if (__install_data->efi) {
        __bootloader_type = "EFI";
        __ddir = __install_data->efi_disk;
    }

    ret = WEXITSTATUS(system(io::sprint("releax-chroot ", __WORK_DIR__, " ", __bootloader_type, " ", __ddir).c_str()));
    if (ret) {
        {
            io::info("Error while installing bootloader");
            LOCK_GUARD(__mutex);
            __mesg = "Failed to installed bootloader";
            __error = true;
            __stop = true;
        }
        return;
    }

    {
        LOCK_GUARD(__mutex);
        __error = false;
        __stop = true;
    }
}

void
util::installer::start(window* caller, data* __install_data)
{
    {
        LOCK_GUARD(__mutex);
        __error = false;
        __stop = false;
    }

    std::thread __runner(
        &installer::run, this, __install_data, caller
    );


    while(true) {
        caller->notify();
        if (__error || __stop) {
            io::info("Breaking loop");
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    __runner.join();
    std::this_thread::sleep_for(std::chrono::milliseconds(120));
    

    io::info("Thread run successfully");
}