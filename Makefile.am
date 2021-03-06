
.INIT :

#CLEANDIRS = etc

SUBDIRS = $(subdirs) \
	librepl \
	zmlib \
	zmchklib \
	libs \
	libjoe \
	src \
	etc \
	scratch \
	j-bayes-tbx \
	j-makemap \
	j-printstats \
	j-scanfile \
	j-greyd \
	j-logserver \
	@MODDIRS@

#	modules/libs 

DBTOOLS = db_archive db_checkpoint db_deadlock db_dump db_hotbackup \
	db_load db_printlog db_log_verify db_recover db_stat db_upgrade db_verify

clean-hook:
	rm -f *~ */*~ */*/*~ */*/*/*~ */*/*/*/*~ #*# */#*# */*/#*# */*/*/#*#

clean-db:
	if [ -d libdb ]; then cd libdb; ./Build clean; cd ..; fi

clean-cache:
	rm -rf autom4te.cache/*

realclean: clean-hook clean-cache distclean

clean-data:
	rm -f $(DESTDIR)@WORKROOT@/j-history
	rm -f $(DESTDIR)@WORKROOT@/j-*data

install-workroot:
	@echo "  * Installing $(DESTDIR)@WORKROOT@"
	@$(mkdir_p) $(DESTDIR)@WORKROOT@ 
	@chmod 755 $(DESTDIR)@WORKROOT@ 
	@chown @USER@ $(DESTDIR)@WORKROOT@ 
	@chgrp @GROUP@ $(DESTDIR)@WORKROOT@ 
	@$(mkdir_p) $(DESTDIR)@WORKROOT@/bin
	@chmod 755 $(DESTDIR)@WORKROOT@/bin
	@chown @USER@ $(DESTDIR)@WORKROOT@/bin
	@chgrp @GROUP@ $(DESTDIR)@WORKROOT@/bin
	@$(mkdir_p) $(DESTDIR)@WORKROOT@/cdb
	@chmod 755 $(DESTDIR)@WORKROOT@/cdb
	@chown @USER@ $(DESTDIR)@WORKROOT@/cdb
	@chgrp @GROUP@ $(DESTDIR)@WORKROOT@/cdb
	@$(mkdir_p) $(DESTDIR)@WORKROOT@/wdb
	@chmod 755 $(DESTDIR)@WORKROOT@/wdb
	@chown @USER@ $(DESTDIR)@WORKROOT@/wdb
	@chgrp @GROUP@ $(DESTDIR)@WORKROOT@/wdb
	@$(mkdir_p) $(DESTDIR)@WORKROOT@/files
	@chmod 755 $(DESTDIR)@WORKROOT@/files
	@chown @USER@ $(DESTDIR)@WORKROOT@/files
	@chgrp @GROUP@ $(DESTDIR)@WORKROOT@/files
	@$(mkdir_p) $(DESTDIR)@WORKROOT@/jgreydb
	@chmod 755 $(DESTDIR)@WORKROOT@/jgreydb
	@chown @USER@ $(DESTDIR)@WORKROOT@/jgreydb
	@chgrp @GROUP@ $(DESTDIR)@WORKROOT@/jgreydb


install-dbtools:
	@echo "  * Installing dbtools"
	@$(mkdir_p) $(DESTDIR)@WORKROOT@/dbtools 
	@chmod 755 $(DESTDIR)@WORKROOT@/dbtools
	@list='$(DBTOOLS)'; for p in $$list ; do \
		$(install_sh_PROGRAM) berkdb/$$p \
			$(DESTDIR)@WORKROOT@/dbtools/j-$$p ;\
	done

install-workdirs:
	@echo "  * Installing Working directories"
	@$(mkdir_p) $(DESTDIR)@SPOOLDIR@
	@chmod 750 $(DESTDIR)@SPOOLDIR@
	@chown @USER@ $(DESTDIR)@SPOOLDIR@ 
	@chgrp @GROUP@ $(DESTDIR)@SPOOLDIR@ 
	@$(mkdir_p) $(DESTDIR)@JGREYDDIR@ 
	@chmod 755 $(DESTDIR)@JGREYDDIR@ 
	@chown @USER@ $(DESTDIR)@JGREYDDIR@ 
	@chgrp @GROUP@ $(DESTDIR)@JGREYDDIR@ 

install-tools:
	@echo "  * Installing Tools"
	$(INSTALL_SCRIPT) bin/jchkmail.init $(DESTDIR)@RCDIR@/@RCSCRIPT@
	$(INSTALL_SCRIPT) bin/jgreyd.init   $(DESTDIR)@RCDIR@/@RCGREYD@
	$(INSTALL_SCRIPT) bin/j-ndc          $(DESTDIR)@prefix@/bin
	$(INSTALL_SCRIPT) bin/get-iana       $(DESTDIR)@WORKROOT@/bin
	$(INSTALL_SCRIPT) tools/j-easy-install $(DESTDIR)@prefix@/bin
	$(INSTALL_SCRIPT) tools/update-wdb-databases $(DESTDIR)@WORKROOT@/bin
	$(INSTALL_SCRIPT) tools/check-mta-conf $(DESTDIR)@WORKROOT@/bin

install-data-hook: install-workroot install-dbtools install-workdirs install-tools checkcf


install-learn :
	@echo "  * Installing Bayes Filter toolbox"
	@$(mkdir_p) $(DESTDIR)@WORKROOT@/bayes-toolbox
	@chmod 755 $(DESTDIR)@WORKROOT@/bayes-toolbox
	@chown @USER@ $(DESTDIR)@WORKROOT@/bayes-toolbox
	@chgrp @GROUP@ $(DESTDIR)@WORKROOT@/bayes-toolbox
	@cp  -pr bayes-toolbox $(DESTDIR)@WORKROOT@/

checkdb :
	@echo " * Check DB versions"
	@tools/update-wdb-databases versions compare

checkcf :
	@tools/check-mta-conf --check

dbsave :
	@echo "  * Dumping work databases"
	@tools/update-wdb-databases -d $(DESTDIR)@WORKROOT@/jgreydb save
	@tools/update-wdb-databases -d $(DESTDIR)@WORKROOT@/wdb     save
	@sleep 1

dbrestore :
	@echo "  * Restoring work databases"
	@tools/update-wdb-databases -d $(DESTDIR)@WORKROOT@/jgreydb restore
	@tools/update-wdb-databases -d $(DESTDIR)@WORKROOT@/wdb     restore
	@sleep 1

dbupgrade :
	@echo "  * Upgrading work databases"
	@tools/update-wdb-databases -d $(DESTDIR)@WORKROOT@/jgreydb upgrade
	@tools/update-wdb-databases -d $(DESTDIR)@WORKROOT@/wdb     upgrade
	@sleep 1

stop-filter :
	@echo "  * Stopping the filters"
	@if [ -x $(DESTDIR)@RCDIR@/@RCSCRIPT@ ] ;\
	then \
		$(DESTDIR)@RCDIR@/@RCSCRIPT@ stop ;\
	fi
	@if [ -x $(DESTDIR)@RCDIR@/@RCGREYD@ ] ;\
	then \
		$(DESTDIR)@RCDIR@/@RCGREYD@ stop ;\
	fi

upgrade : all
	$(MAKE) $(AM_MAKEFLAGS) stop-filter
	echo $(MAKE) $(AM_MAKEFLAGS) dbsave
	$(MAKE) $(AM_MAKEFLAGS) install
	echo $(MAKE) $(AM_MAKEFLAGS) dbrestore
	$(MAKE) $(AM_MAKEFLAGS) dbupgrade
	cd etc && $(MAKE) $(AM_MAKEFLAGS) upgrade 

