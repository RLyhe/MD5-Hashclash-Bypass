# Project - MD5 Hashclash to Bypass RADIUS
The following tool `hashclash` comes from [@cr-marcstevens](https://github.com/cr-marcstevens) 
(https://github.com/cr-marcstevens/hashclash)

## Requirements

- C++11 compiler (e.g. g++)
- make
- autoconf & automake & libtool

  `sudo apt-get install autoconf automake libtool`
  
- zlib & bzip2 libraries

  `sudo apt-get install zlib1g-dev libbz2-dev`
  
- Optional: CUDA
  
## Building (automatic)

- Run build.sh script

   `./build.sh`

### in ``hashclash/scripts/cpc.sh``
Check if the ``


#### Freeradius Command

```
radtest collision1 password 127.0.0.1 0 SharedSecret
```