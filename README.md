slb2unpack
========

Unpacks a SLB2 container file

Credits
-------
Hykem (forked from ps4tools)
flat_z (original Python scripts for PUP and PKG unpacking)

CHANGELOG
--------

* slb2unpack tool

    - pupunpack: renamed to slb2unpack and removed references to PUP.
    - slb2unpack: more error checks. Fixed potential read bug since fread return not checked.
	
* First Release

	- pupunpack: splits PS4UPDATE.PUP and exposes inner PUP files (encrypted).
	- unpkg: unpacks retail/debug PKG files while collecting data and dumping internal files (mostly a C port of flat_z's Python script, at the moment).
