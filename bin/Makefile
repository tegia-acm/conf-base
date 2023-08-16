include ../../../Makefile.variable

CPPFLAGS += -I ~/tegia/vendors/ftpclient/FTP
LIBS += ~/tegia/vendors/ftpclient/build/Release/lib/libftpclient.a

# =============================================================================================================
#
# BUILD
#
# =============================================================================================================

base:
	@echo " "
	@echo "------------------------------------------------------------"
	@echo "BASE CONFIGURATION: \033[32;1m BUILD \033[0m"
	@echo "------------------------------------------------------------"
	@echo " "

	make libA2Scheduler
	make libA2Events
	make libA2Files
	make libA2Mailer
	make libA2Names
	make libA2Sphinx

# =============================================================================================================
#
# INSTALL SOURCE
#
# =============================================================================================================

install:
	@echo " "
	@echo "------------------------------------------------------------"
	@echo "BASE CONFIGURATION: \033[32;1m CONFIGURE SOURCE \033[0m"
	@echo "------------------------------------------------------------"
	@echo " "

	bash ./install.sh
	make base

	@echo " "
	@echo "BASE CONFIGURATION: \033[32;1m INSTALL SUCCESS \033[0m"
	@echo " "


# =============================================================================================================
#
# CREATE BINARY DEB PACKAGE
#
# =============================================================================================================

base-bin-deb:
	make base

	@echo " "
	@echo "------------------------------------------------------------"
	@echo "BASE CONFIGURATION: \033[32;1m CREATE BINARY DEB PACKAGE \033[0m"
	@echo "------------------------------------------------------------"
	@echo " "

	cp -r *.so ../deb/tegia-base-bin/etc/tegia/base/bin/
	cp base.config.json ../deb/tegia-base-bin/etc/tegia/base/bin/base.config.json
	cp ../db/install.sql ../deb/tegia-base-bin/etc/tegia/base/db/install.sql
	cp ../db/uninstall.sql ../deb/tegia-base-bin/etc/tegia/base/db/uninstall.sql
#	Собираем deb-пакет
	@echo "\033[32mmake deb-package\033[0m"
	cd ../deb/ && fakeroot dpkg-deb --build tegia-base-bin
	cp ../deb/tegia-base-bin.deb ../../../debs/tegia-base-bin.deb


# =============================================================================================================
#
# CREATE SOURCE DEB PACKAGE
#
# =============================================================================================================

base-source-deb:
	@echo " "
	@echo "------------------------------------------------------------"
	@echo "BASE CONFIGURATION: \033[32;1m CREATE SOURCE DEB PACKAGE \033[0m"
	@echo "------------------------------------------------------------"
	@echo " "

#	Собираем deb-пакет
	@echo "\033[32mmake deb-package\033[0m"
	cd ../deb/ && fakeroot dpkg-deb --build tegia-base-source
	cp ../deb/tegia-base-source.deb ../../../debs/tegia-base-source.deb



# =============================================================================================================
#
# BUILD ACTORS
#
# =============================================================================================================

libA2Scheduler:
	g++ -c -o A2Scheduler.o         ../actors/A2Scheduler/A2Scheduler.cpp $(Flag) -fPIC
	g++ -c -o ccronexpr.o           ../vendors/ccronexpr/ccronexpr.c $(Flag) -fPIC -DCRON_USE_LOCAL_TIME
	g++ -shared -fPIC -o libA2Scheduler.so ccronexpr.o        \
	    A2Scheduler.o -fPIC $(Flag)

libA2Events:
	g++ -c -o A2Events.o            ../actors/A2Events/A2Events.cpp $(Flag) -fPIC
	g++ -shared -fPIC -o libA2Events.so        \
	    A2Events.o $(Flag)

libA2Files:
	g++ -c -o A2Files.o             ../actors/A2Files/A2Files.cpp $(Flag) -fPIC
	g++ -shared -fPIC -o libA2Files.so          \
	    A2Files.o -lfmt -lnlohmann_json_schema_validator $(Flag)

libA2Mailer:
	g++ -c -o A2Mailer.o             ../actors/A2Mailer/A2Mailer.cpp $(Flag) -fPIC
	g++ -shared -fPIC -o libA2Mailer.so         \
	    A2Mailer.o -lnlohmann_json_schema_validator $(Flag)

libA2Names:
	g++ -c -o A2Names.o              ../actors/A2Names/A2Names.cpp $(Flag) -fPIC
	g++ -shared -fPIC -o libA2Names.so         \
	    A2Names.o $(Flag)

libA2Sphinx:
	g++ -c -o A2Sphinx.o              ../actors/A2Sphinx/A2Sphinx.cpp $(Flag) -fPIC
	g++ -shared -fPIC -o libA2Sphinx.so         \
	    A2Sphinx.o $(Flag)
