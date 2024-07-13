# Installing your virtual machine

If you are using a MacBook or a Windows machine we strongly suggest that you use
- [VirtualBox](https://www.virtualbox.org/): Windows and Mac Intel (x86) CPU
- [VMware Fusion Player](https://customerconnect.vmware.com/evalcenter?p=fusion-player-personal-13): Mac M1/M2 CPUs

To find if your Mac has an Intel CPU, click on the Apple symbol at the top right, and select "About This Mac".

If your computer is something other than an x86 Windows machine or a Macintosh (x86 or M1/M2) you're on your own, although we'll try to give you a hand.

### VirtualBox (Windows, x86 Macintosh)

- Download the Windows or Macintosh version of [VirtualBox](https://www.virtualbox.org/) as appropriate.
- Install by double-clicking, choosing default options
- Download the virtual machine image: [`cs5600_x86-64.ova`](https://www.khoury.northeastern.edu/~pjd/cs5600_x86-64.ova)
    Note that you may need to use "Save as" or similar, depending on how your browser is configured.
- In VirtualBox, select "File" -> "Import Appliance", and select `cs5600_x86-64.ova`

If you have a Macintosh and you haven't changed the default security settings, you may have to go into System Settings -> Privacy & Security and click the radio button to allow applications downloaded from "App Store and identified developers".

### VMware Fusion Player (M1/M2 Macintosh)

register a free Personal User License of VMware Fusion Player 13:
- Go to [VMware Fusion Player](https://customerconnect.vmware.com/evalcenter?p=fusion-player-personal-13);
- select “License & Dowload” tab; click “create an account” (if you don’t have one);
- follow the instructions to create a VMware account;
- login to your account;
- go to [VMware Fusion Player](https://customerconnect.vmware.com/evalcenter?p=fusion-player-personal-13) again to get your Personal User License.
- copy the license key, and paste it when Fusion Player asks for your license.

Download the virtual machine image: [CS5600_arm64.vmwarevm.zip](https://www.khoury.northeastern.edu/~pjd/CS5600_arm64.vmwarevm.zip)

Your browser may have unzipped the file when downloading (probably only Safari does this); otherwise you'll have to unzip the file yourself, yielding a directory named `CS5600_arm64.vmwarevm`.
(you can move that directory or leave it in Downloads)
In Fusion Player, select File -> Open, then navigate to and select the `.vmware` directory.

### Common steps

Whether you are using VirtualBox or Fusion Player, you may see a few warnings or dialog boxes when you first boot up; these can usually be ignored. (tell Fusion that you copied the VM)

The VM should have an account named `cs5600`, with password `111111`.
You can use that account to administer the VM, and you'll need the password.

Two things that you may wish to do:

**Enable SSH from your laptop to your VM:**
Open a terminal and run the following commands:
```
sudo apt update
sudo apt install openssh-server
ip addr
```
You should see an address that looks something like 192.168.240.128.
That's a private IP address between your laptop and your VM, and you can SSH to it from your laptop.
To make this simpler, you can configure it in SSH:
```
# add to ~/.ssh on laptop
Host cs5600-vm
  Hostname 192.168.240.128
  User cs5600
```
You can also use the `ssh-copy-id` command to copy your public key over to the VM, so that you don't have to type your password to use it:
```
% ssh-copy-id cs5600-vm
  ... noisy output ...
pjd@192.168.240.128's password:  <type password here>
  ... more noisy output ...
```

**Create a personalized login:**
You can create a new user from the terminal in your VM:
```
$ sudo adduser shays
[sudo] password for pjd: 
Adding user `shays' ...
Adding new group `shays' (1002) ...
Adding new user `shays' (1002) with group `shays' ...
Creating home directory `/home/shays' ...
Copying files from `/etc/skel' ...
New password: 
Retype new password: 
passwd: password updated successfully
Changing the user information for shays
Enter the new value, or press ENTER for the default
	Full Name []: Daniel Shays
	Room Number []: 
	Work Phone []: 
	Home Phone []: 
	Other []: 
Is the information correct? [Y/n] 
```
Then we add the new user to the `sudo` group so that it has sudo permission:
```
sudo usermod -a -G sudo shays
```
