Clone
  git clone https://github.com/Surrog/tabletop-civlike.git
  git submodule update --init
  Dependancy:
    boost, one of the latest version
    xtsslib, included in the repository
  
Cmake generation
  Variable to set on windows:
  BOOST_ROOT : the root directory where boost sources are stored
  BOOST_VERSION : the boost version used, eg : 1_63
 
Compilation
  make or visual studio command
  
Usage
  resolver_server [--help] [-o <output_path>] [-i] input_path
  Allowed options:
    --help                produce this help message
    --o arg               <PATH> output directory
    --i arg               <PATH> input directory
