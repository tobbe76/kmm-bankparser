# kmm-bankparser
kmymoney plugin, fetch bank statements using webkit

Supported banks:
* SEB Sverige
* Norwegian Bank

Some support
* Handelsbanken
* Swedbank

# How to build
mkdir build
cd build
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX:PATH=/usr -DQT_QT_INCLUDE_DIR=/usr/include/qt4/ -DQT_SELECT=4 ../bankparser
make -j2 install
