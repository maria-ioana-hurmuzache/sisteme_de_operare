#!/bin/bash
### My id command. ###

if [ $# -eq 0 ]
then
    echo "Eroare: parametri insuficienti! Nu ati specificat UID-ul."
    exit 1  # Fail!
fi

grep ":$1:" /etc/passwd  >/dev/null
if [ $? -eq 0 ]
then
    echo -n Numele real:
    grep ":$1:" /etc/passwd | cut -d: -f5 | cut -d, -f1

    echo -n Numele de cont:
    grep ":$1:" /etc/passwd | cut -d: -f1

    echo -n Grupul principal din care face parte:
    grep :$(grep ":$1:" /etc/passwd | cut -d: -f4): /etc/group | cut -d: -f1

    echo -n Grupurile secundare de care apartine:
    grep -w $(grep ":$1:" /etc/passwd | cut -d: -f1) /etc/group | cut -d: -f1
else
    echo "Eroare: nu exista nici un utilizator avand $1 drept UID."
    exit 2  # Fail!
fi

exit 0  # Succes!

