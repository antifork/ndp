#
# $Id$
# %ndp: Makefile 
#
# Copyright (c) 1999 Bonelli Nicola <bonelli@antifork.org>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#


all: ndp 
ndp: src/ndp

version.o: version.h

version.h: ./VERSION
	@rm -f include/$@
	@sed -e 's/.*/#define VERSION "&"/' ./VERSION > include/$@
	@./config.guess | sed -e 's/.*/#define CPU_MFR_OPSYS "&"/' >> include/$@
	@cat INFO | awk '{print "#define AUTHOR \"" $$2 " " $$3 "\"" }' >> include/$@
	@cat INFO | awk '{print "#define MAIL_SUPPORT \"" $$4 "\"" }' >> include/$@

src/ndp: version.h
	@./header
	cd src/ && make
clean:
	rm -f src/*BAK src/*~ src/*.o ndp passwd_ndp ndp.pid 
distclean:
	rm -f *.log *.cache src/Makefile src/*BAK src/*~ include/{defs,version}.h src/*.o ndp passwd_ndp ndp.pid 
