# iPDC
This project is an extension to [iPDC](https://sourceforge.net/projects/iitbpdc/)


General Information
-------------------

	iPDC consists of 3 different modules, 

          iPDC - Phasor Data Concentrator compliant with IEEEC37.118 synchrophasor standard, 
          DBServer - which is used to store the data in MySQL database, and 
          PMUSimulator - which simulates the Phasor Measurement Unit compliant with IEEEC37.118 synchrophasor standard.

	This version works in Unix and Unix-like operating systems. It uses GCC compiler, GTK+ graphical user interface library & also POSIX thread library, and MySQL as database server. 


Dependencies
-------------

	gcc
	mysql-server-5.0
	mysql-client-5.0.
	mysql-common
	libmysqlclient16*
	GTK+3
	glade 3.6.7 (optional for development)
	NTP-client  (For synchronization of system clock)


# INSTALLATION:


     Dependencies installation: Installation of glade internally installed the GTK+ libraries and there is no need to explicit installation of GTK+. NTP is recommended for better results by iPDC and PMU Simulator.

          su/sudo apt-get update (recommended)
          su/sudo apt-get install mysql-server-5.0 (or latest available version)
          su/sudo apt-get install mysql-client-5.0 (or latest available version)
          su/sudo apt-get install mysql-common
          su/sudo apt-get install libmysqlclient16 (or latest available version)
          su/sudo apt-get install libmysqlclient16-dev (or latest available version)
          su/sudo apt-get install glade
          su/sudo apt-get install ntp (http://howto.eyeoncomputers.com/ubuntu/install-ntp/)
		  su/sudo apt-get install libosmgpsmap-1.0-1 gir1.2-osmgpsmap-1.0
		  su/sudo apt-get install libosmgpsmap-1.0-dev
		  sudo apt-get install libgee-0.8-dev

     For detailed explanation of iPDC, DBServer, and PMUSimulator working refer the User Manual. Available on website. Make sure you have done with the dependency installation before go for installation of iPDC software.

     A) iPDC Installation -> 

  		Multiple instance can be run on same machine by ONLY one time installation.
          First go to the source folder on terminal then,

	        1. cd iPDC.x.y/iPDC.x
			2. su/sudo make install (It will be needing root permision to create folder & copying file on them)
			3. Run iPDC via terminal or you could found its shortcut on menu named iPDC.


     B) Install DBServer ->

          DBServer can be installed on the same machine or a different machine.
          If DBServer is to be installed on a different machine then copy iPDC.x.y/DBServer to that machine(cp iPDC.x.y/DBServer /PATH....). Install dependencies like demysql-server-5.0, mysql-common, libmysqlclient15off, mysql-client-5.0. on that machine and Then follow the steps given below.

	          1. cd /PATH/iPDC.x.y/DBServer.x.y
	          2. mysql -u root -p <"Db.sql"
	          3. make
			4. ./DBServer or DBServer (accesseble only from folder)

     C) Install PMUSimulator ->

          If you need to run simulator on different from iPDC machine then Copy the PMU Simulator on that machine.
          Single installation of PMU Simulator will work in case you need to simulate multiple PMUSimulator on the same machine.

	          1. cd /PATH/iPDC.x.y/PMUSimulator.x.y
	          2. su/sudo make install (It will be needing root permision to create folder & copying file on them)
			3. Run - PMU via terminal or you could found its shortcut on menu name PMU Simulator.


USAGE
------

     i) When iPDC acts as a Client ->

	     1. Run DBServer on terminal of the machine where DBServer is installed. This will be listening on port 9000 for data & CFG frames from iPDC.

	     2. Run PMU Simulator in the machine where PMUSimulator is installed. You can run the PMU Simulator from anywhere in the system. Enter the PMU Port numbers for TCP and UDP. Port should be other than 9000, as 9000 is used by DBServer. 

	     3. Run iPDC and enter the details like TCP Port, UDP Port.
			These are the ports on which PDC would listen to command frames from other PDC's, or applications.
			Also enter PDC ID, that will be use for authenticate to other ends PDC's.
			Enter the IP Address of the machine where DBServer is running. (if local, may use like 127.0.0.1)
			Now add the PMU/Source devices by entring their minimal details.
			As soon as the PMU details are entered, a command frame is sent to machine where the PMU/Simulator is running.
			And communication between iPDC and PMUSimulator (PMU) will take place as follows.


          4. Communications
    
	          1. iPDC sends a command frame to PMU, requesting its configuration.

	          2. At PMU on receiving of a request for Configuration frame, PMU send back the latest configuration frame iPDC.
           
	          3. iPDC then send a command frame to requesting PMU to start data frames transmission.

	          4. PMU on reception of the command frame for send data would reply with data frames.

		          iPDC  -----------> 	PMU (Command to send CFG)
		          iPDC  <----------- 	PMU (CFG Sent)
		          iPDC  -----------> 	PMU (Command to send Data)
		          iPDC  <----------- 	PMU (Data Sent)
		          iPDC  <----------- 	PMU (Data Sent)
		          iPDC  <----------- 	PMU (Data Sent)
		          iPDC  -----------> 	PMU (Command to stop Data)

			 5. In the above mentioned PMU-iPDC communication, iPDC would act as a client and PMU as server.

     ii) When iPDC acts as a Server ->

		When iPDC acts as a Server, it sends the combined frames (data and Configuration) to other iPDC on requesting.
		The details of these other end iPDC's need to be entered first. In this case the iPDC would work like as PMU Server.
		iPDC receives a command frames from other iPDC it would first authenticate the request.
		If the request is from authentic iPDC then combined frame would sent to respective iPDC. Remaining same as iPDC-PMU Communication.
                                                                                                                         /
		More details of the s/w can be found in the Technical Report on iPDC on the website.


UNINSTALL
----------

     Uninstall iPDC ->
		cd iPDC.x.y/iPDC.x/
		su/sudo make uninstall   (will be needing roots permission)

     Uninstall DBServer ->
		cd iPDC.x.y/DBServer.x/
		make clean

     Uninstall PMUSimulator ->
		cd iPDC.x.y/PMUSimulator.x/
		su/sudo make uninstall   (will be needing roots permission)


Future Planning
--------------

    We are planning of having following major Future milestone. Help and suggestions are required from open source community.

    1. Visualization applications for phasor data.
    2. Porting of complete iPDC software on Windows Environment 
    3. Import/export Data file in comtrade format.
        
	These are our major thrust area, currently the only way to study the data is in MySQL database, or in DBserver terminal, but it is not sufficient! We intend to develop a interface (something like charting Visualization) in which we can render the data in  real time.

	To increase the usability and versatility of the application second objective is also appears to be quite essential due to the popularity of the windows environment.


Can I HELP??
-----------------

        Yes, SURE you can help us..!!  in fact we really NEED you and will be happy to have your support.
        We have brought the project up to this level but as all other open source software project 
        this project also needs programmer/developer community's Love, Care & Support.
        So please FEEL FREE to join us - support us – Contact us in any ways you wish...


How Can I HELP?
------------------

	Currently you can help us (mainly) in following ways
        
	1. If you are good UI designer (QT, JNI, GTK, Pyton, etc... anything) then please help us design more user friendly and attractive GUI for iPDC.
	2. If you are good (or even rookie will also do, because no body is a born programmer!!) at Visualization programming like charting API, graphics or any other data rendering methods which can handle large amount of data and possibilily in real time using python, C/C++, Perl etc.
	3. Apart from above mentioned things, if there is anything else you think needs to be done for improvement(OBVIOUSLY!!) of the project and in that you can help us, please DO CONTACT US. We will be more than happy to have your support.
	4. If possible please let us know where you are using iPDC software. This really help us.	


PROBLEMS & FAQ
---------------

	If you get into problems, find this application useful or find bug in the software, just feel anything except nothing about it.
	Please feel free to contact us through bug report on website's discussions page or issue tracker page or via email.
	Our motivation for further work grows with every mail.
	Join the forum/issue tracker on the iPDC websites to get latest updates about iPDC software. 


How to Reach Us:-
------------------
    	Use discussion forum here in github.
    

Credits
--------

	C and GCC for programming language and compiler respectively.
	MySQL database for iPDC Database.
	GDB as debugging tool for software. 
	Glade & GTK+ for GUI functionality of software.
	Dia software for image creation for report and docs.
	Kile for report generation and also for presentation.
	Wireshark for IEEE C37.118 protocol understanding and testing.
	www.photoserch.com for Royalty Free Images.
	www.onlinelogomaker.com for logo creation of software.
	And many other open source softwares and websites/forums that has been contributed to iPDC project directly or indirectly.


Note
-----

	CFG stands for Configuration Frame.
	If NTP Client is not running on the machines where iPDC, PMU Simulator, and DBserver running, so they will not be time synchronized. Your final results may be incorrected.
	NTP Client and your near server of NTP is highly recommended. You can also use IIT Bombay's NTP Server.
	Commands for NTP has not been added into makefile, so if you want NTP synchronization then please install NTP client and start it. For installation of NTP-client and NTP-server configuration on your Linux machine please visit respective community website.
