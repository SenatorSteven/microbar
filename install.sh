
# install.sh
#
# MIT License
#
# Copyright (C) 2019 Stefanos "Steven" Tsakiris
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

#!/bin/bash

NAME="install.sh";
true=1;
false=0;

function main(){
	[ $true                             ] && { local name="microbar";                                                                                        } || :;
	[ $true                             ] && { local user="$(getRegularUser)";                                                                               } || :;
	[ $true                             ] && { local home="$(getent passwd "$user" | cut -d : -f 6)";                                                        } || :;
	[ $true                             ] && { cd "${BASH_SOURCE%/*}";                                                                                       } || :;
	[ "$EUID" != "0"                    ] && { printf "$NAME: installer not running as root\n";                                               return 1;      } || :;
	[ ! -n "$user"                      ] && { printf "$NAME: could not find regular user\n"; 1>&2;                                           return 1;      } || :;
	[[ ! -n "$home" || "$home" == "/"  ]] && { printf "$NAME: could not find regular user home directory\n"; 1>&2;                            return 1;      } || :;
	[ ! -f "$NAME"                      ] && { printf "$NAME: could not find $NAME directory\n" 1>&2;                                         return 1;      } || :;
	[ ! -f "compile.sh"                 ] && { printf "$NAME: could not find $name compilation file\n" 1>&2;                                  return 1;      } || :;
	[ ! -d "etc/$name"                  ] && { printf "$NAME: could not find $name system configuration directory\n" 1>&2;                    return 1;      } || :;
	[ ! -d ".config/$name"              ] && { printf "$NAME: could not find $name user configuration directory\n" 1>&2;                      return 1;      } || :;
	[ ! -f "/tmp/$name/output/$name"    ] && { printf "$NAME: could not find $name executable, compiling...\n"; sudo -u "$user" ./compile.sh;                } || :;
	[ ! -f "/tmp/$name/output/$name"    ] && { printf "$NAME: could not compile $name executable\n"; 1>&2;                                    return 1;      } || :;
	[ $true                             ] && { rm -rf "/bin/$name" "/etc/$name" &> /dev/null;                                                                } || :;
	[ $true                             ] && { mkdir -p "/bin" "/etc" &> /dev/null;                                                                          } || :;
	[ $true                             ] && { sudo -u "$user" mkdir -p "$home/.config" &> /dev/null;                                                        } || :;
	[ $true                             ] && { cp "/tmp/$name/output/$name" "/bin/$name" &> /dev/null;                                                       } || :;
	[ $true                             ] && { cp -r "etc/$name" "/etc/$name" &> /dev/null;                                                                  } || :;
	[ ! -d "$home/.config/$name"        ] && { sudo -u "$user" cp -r ".config/$name" "$home/.config/$name" &> /dev/null;                                     } || :;
	[ $true                             ] && { rm -rf "/tmp/$name" &> /dev/null;                                                                             } || :;
	[ $true                             ] && { printf "done\n";                                                                               return 0;      } || :;
}
function getRegularUser(){
	[ $true                             ] && { local user="${SUDO_USER:-$USER}";                                                                             } || :;
	[[ -n "$user" && "$user" != "root" ]] && { printf "%s\n" "$user";                                                                         return $true;  } || :;
	[ $true                             ] && { user="$(logname)";                                                                                            } || :;
	[[ -n "$user" && "$user" != "root" ]] && { printf "%s\n" "$user";                                                                         return $true;  } || :;
	[ $true                             ] && { user="$(loginctl list-users --no-legend 2> /dev/null | awk 'NR==1 {print $2; exit}')";                        } || :;
	[[ -n "$user" && "$user" != "root" ]] && { printf "%s\n" "$user";                                                                         return $true;  } || :;
	[ $true                             ] && { printf "\n";                                                                                   return $false; } || :;
}

main "$@";
exit $?;

