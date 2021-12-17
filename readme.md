# cpp_loader [![C++](https://img.shields.io/badge/language-C%2B%2B-%23f34b7d.svg)](https://en.wikipedia.org/wiki/C%2B%2B) [![Windows](https://img.shields.io/badge/platform-Windows-0078d7.svg)](https://en.wikipedia.org/wiki/Microsoft_Windows)

a c++ loader

## requirements
* curl
* vmprotect sdk

## notes:
* edit the vmprotect path in project settings -> build events -> post-build event to yours

* place "encrypter.exe" in "C:\output" (create a new folder).
to use it, simply run "encrypter.exe" as administrator and type the dll name

* there is also an example php file to get the dll output in the loader