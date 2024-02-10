There are two ways to apply the patches

First Method: Using quilt command

step 1: Install the command 'quilt'

		# sudo apt-get install quilt

step 2: Download the barebox-2013.07.0 source code.

		# wget http://barebox.org/download/barebox-2013.07.0.tar.bz2

step 3: Extract the source code.

		# tar -xvf barebox-2013.07.0.tar.bz2

step 4: Go to extracted folder

		# cd barebox-2013.07.0/

step 5: Create a directory with the name "patches" in the extracted barebox-2013.07.0 directory.

		# mkdir patches

step 6: Go the patches directory

		# cd patches

step 7: Download all the patches for barebox-2013.07.0 from the phytec ftp site

		# wget ftp://ftp.phytec.de/pub/Products/India/Rana-AM335x/Linux/PD13.0.0/src/patches/barebox-2013.07.0/patches/*

step 8: Go to the barebox-2013.07.0 top directory and run quilt command.

		barebox-2013.07.0 # quilt push -a

Second Method: Using patch command

step 1: Follow the steps 1 to 4 as mentioned in the First method.

step 2: Download all the patches for barebox-2013.07.0 from the phytec ftp site

		# wget ftp://ftp.phytec.de/pub/Products/India/Rana-AM335x/Linux/PD13.0.0/src/patches/barebox-2013.07.0/patches/*
	
step 3: Apply the patches order wise from 0000,0001... using "patch" command

		# patch -p1 < [patch-file-name].patch

