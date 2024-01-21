# Build and debug session for RaspberryPi OS
RaspberryOS default approach assumes that you will build kernel and modules directly on the Raspberry. So... It works but the scenario becomes the same as
for the host (see [Host story](story_host.md)
Lets talk about [crosscompillation approach](https://www.raspberrypi.com/documentation/computers/linux_kernel.html#cross-compiling-the-kernel) It works faster but adds complexity working with HW databuses
But from other side this is why does YALDA exist for. Not to add complexity but to provide universal vendor agnostic approach and to work with databuses UDD exist.
I will just follow instructions from the guide and adopt YALDA to work with it
```bash
git clone --depth=1 https://github.com/raspberrypi/linux
```

![Raspberry configuration](images/story_rpios-config.gif)

Then you need to build the kernel and other components within YALDA environment
```bash
yalda sync
yalda build
```
After this step YALDA is ready to be used to build and debug your module

![YALDA configuration](images/story_debug-session.gif)

Finally when your module is done you need to compile the sources right on the device of recompile compete kernel with modules.
Use the guide above
