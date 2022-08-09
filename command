

top -n1 -b | awk -F ' ' $'{print $1","$2","$9","$11; system("cat /proc/"$1"/cmdline; cat /proc/"$1"/status | grep PPid | awk -F \' \' \'{print $2}\'; cat /proc/"$1"/status | grep Name | awk -F \' \' \'{print $2}\'")}'op -n1 -b | awk -F ' ' $'{print $1","$2","$9","$11; system("cat /proc/"$1"/cmdline; cat /proc/"$1"/status | grep PPid | awk -F \' \' \'{print $2}\'; cat /proc/"$1"/status | grep Name | awk -F \' \' \'{print $2}\'")}'
