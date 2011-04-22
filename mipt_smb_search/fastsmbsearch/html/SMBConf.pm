# This file is part of Fast SMB Search
#
# Copyright 2001, 02, 03, 04   Alexander Vodomerov
#
# Fast SMB Search is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# Fast SMB Search is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
# 02111-1307, USA.

package SMBConf;

use strict;
use warnings;
use diagnostics;
use vars qw(@ISA @EXPORT);

BEGIN {
	use Exporter;
	our ($VERSION, $ISA, @EXPORT);
	
	$VERSION = '0.3';
	
	@ISA = qw(Exporter);
	@EXPORT = qw($db_name $db_user $db_pass $fsearch_socket);
}

our $db_name = "DBI:mysql:smbsearch";
our $db_user = "smb";
our $db_pass = "";
our $fsearch_socket = "/tmp/fastsmb.sock";

END { }

# Currently no initialization is done

# return 1 as success
1;

