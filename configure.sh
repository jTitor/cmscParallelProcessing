# git clone https://github.com/jTitor/cmscParallelProcessing.git
# cd cmscParallelProcessing
# bash configure.sh

LIBJPG_DIR="Libraries/graphics435/jpeglib"
if [ ! -d "$LIBJPG_DIR" ]; then
  # need to download library
  echo "downloading jpeg lib"
  wget http://ijg.org/files/jpegsrc.v9b.tar.gz
  echo "unpacking lib"
  tar xvzf jpegsrc.v9b.tar.gz
  rm jpegsrc.v9b.tar.gz
  mv jpeg-9b jpeglib
  mv jpeglib Libraries/graphics435/
  echo "configuring lib"
  cd Libraries/graphics435/jpeglib/
  ./configure # probably need to do this, not sure
  cd ../../..
else
  echo "already have jpeg lib"
fi
