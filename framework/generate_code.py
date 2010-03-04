#! /usr/bin/python
# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import sys

from optparse import OptionParser
from pyplusplus import module_builder


def do_main():

  op_parser = OptionParser()
  op_parser.add_option("-o","--outputfile",type="string",dest="outputfile",action="store")
  op_parser.add_option("-m","--modulename",type="string",dest="modulename",action="store")
  op_parser.add_option("-I","--includepaths",type="string",dest="include_paths",action="store",default='')
  op_parser.add_option("-D","--define",type="string",dest="define_symbols",action="store",default='')
  op_parser.add_option("--gccxml_bin",type="string",dest="gccxml_bin",action="store",default='/usr/bin/gccxml')


  (options,input_files) = op_parser.parse_args(sys.argv)

  if options.outputfile == None :
    op_parser.error("Output file not specified")

  if options.modulename == None:
    op_parser.error("Module name not specified")  

  if os.path.exists(options.outputfile):
    os.remove(options.outputfile)

    
  print 'parsing input files',input_files[1:len(input_files)]


  mb = module_builder.module_builder_t(
	  files = input_files[1:len(input_files)]
	  , gccxml_path= options.gccxml_bin
	  , include_paths = [options.include_paths]
	  , define_symbols = [options.define_symbols]
	  ) 
	  
  #framework.is_abstract = True;
  #number.add_fake_constructors( number.calldefs( 'create' ) )

  #Now it is the time to give a name to our module
  mb.build_code_creator( module_name=options.modulename )

  #It is common requirement in software world - each file should have license
  mb.code_creator.license = '//Boost Software License( http://boost.org/more/license_info.html )'

  #I don't want absolute includes within code
  mb.code_creator.user_defined_directories.append( os.path.abspath('.') )

  #And finally we can write code to the disk
  mb.write_module( os.path.join( os.path.abspath('.'), options.outputfile ) )

do_main()