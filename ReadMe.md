


# ding-driver
### - A virtual network driver for real world network simulation

**Version** - ding-driver v0.1

**Project is still under active development and multiple portions are yet to mature**

**Developer & Maintainer** - [Rishi Tharun](https://linkedin.com/in/rishitharun03) - <<vrishitharunj@gmail.com>><br>
<br>
>NOTE:
> * Target system - **Linux**
> * Root access is required to run
> * Kernel headers are required to run
> * GNU Compiler Collection is required to run
> * Install **kernel headers** and **gcc** before proceeding to build
> * Driver written and tested with Linux Kernel v5.10

### Configurations
* Use the config file `dconfig.cfg` to configure the driver (Basics) and simulation environment (TBD)
* Sample config file `dconfig.cfg` is provided with more details
* It is necessary, but not mandatory to have a config file
* In the absence of a config file, default configs will be considered
* Refer `dconfig.cfg` for default configs

> NOTE: Configuration concept is not yet matured and may undergo more major revisions

### To Build and Load:
`make`

### ding-drivers
* Run `make` from the project root
* If successful, two new network interfaces will be loaded
* Using `ifconfig`, the interfaces `ding` and `dingdummy` can be seen
* `ding` is the virtual interface, that will be used for simulations
* `dingdummy` is just a support interface for `ding`
* For all simulations, use the `ding` interface **ONLY**
* If `dconfig.cfg` is used, it can be seen that `ding` is configured as per the file
* `dingdummy` will be configured automatically based on the scenarios

> *Simulate the traffic on your own, as you wish !*

