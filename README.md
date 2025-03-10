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
Check if the ``maxblocks < 3`` 

## FreeRADIUS

### Configuration

1. Install FreeRADIUS:
   ```sh
   sudo apt-get install freeradius
   ```

2. Edit the `clients.conf` file to add a client:
   ```sh
   sudo nano /etc/freeradius/3.0/clients.conf
   ```
   Add the following configuration:
   ```conf
   client localhost {
       ipaddr = 127.0.0.1
       secret = SharedSecret
       require_message_authenticator = no
       nas_type = other
   }
   ```

3. Edit the `users` file to add a user:
   ```sh
   sudo nano /etc/freeradius/3.0/users
   ```
   Add the following configuration:
   ```conf
   collision1 Cleartext-Password := "password"
   ```

4. Restart FreeRADIUS:
   ```sh
   sudo systemctl restart freeradius
   ```

#### in ``radiusd.conf``, set ``message_authenticator=no``, ``proxy_request=yes``

#### Freeradius Command

```
radtest collision1 password 127.0.0.1 0 SharedSecret
```

#### Catch Package

Use ``wireshark`` to catch packge, and check the content in raw format.
