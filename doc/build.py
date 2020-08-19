#!/usr/bin/env python3
# Build the documentation.

import errno, os, shutil, sys, tempfile
from subprocess import check_call, check_output, CalledProcessError, Popen, PIPE
from distutils.version import LooseVersion

versions = ['0.1.0']

def pip_install(package, commit=None, **kwargs):
  "Install package using pip."
  min_version = kwargs.get('min_version')
  if min_version:
    from pkg_resources import get_distribution, DistributionNotFound
    try:
      installed_version = get_distribution(os.path.basename(package)).version
      if LooseVersion(installed_version) >= min_version:
        print('{} {} already installed'.format(package, min_version))
        return
    except DistributionNotFound:
      pass
  if commit:
    package = 'git+https://github.com/{0}.git@{1}'.format(package, commit)
  print('Installing {0}'.format(package))
  check_call(['pip', 'install', package])

def create_build_env(dirname='virtualenv'):
  # Create virtualenv.
  if not os.path.exists(dirname):
    check_call(['virtualenv', dirname])
  import sysconfig
  scripts_dir = os.path.basename(sysconfig.get_path('scripts'))
  activate_this_file = os.path.join(dirname, scripts_dir, 'activate_this.py')
  with open(activate_this_file) as f:
    exec(f.read(), dict(__file__=activate_this_file))

  # Import get_distribution after activating virtualenv to get info about
  # the correct packages.
  from pkg_resources import get_distribution, DistributionNotFound

  pip_version = get_distribution('pip').version
  if LooseVersion(pip_version) < LooseVersion('1.5.4'):
    print("Updating pip")
    check_call(['pip', 'install', '--upgrade', 'pip'])

  try:
    distribute_version = get_distribution('distribute').version
    if LooseVersion(distribute_version) <= LooseVersion('0.6.24'):
      print("Updating distribute")
      check_call(['pip', 'install', '--upgrade', 'distribute'])
  except DistributionNotFound:
    pass

  # Install Sphinx and Breathe.
  pip_install('breathe', None, min_version="4.15.0")
  pip_install('sphinx', None, min_version="3.1.0")

def build_docs(sphinx_executable='sphinx-build', version='dev', **kwargs):
  doc_dir = kwargs.get('doc_dir', os.path.dirname(os.path.realpath(__file__)))
  work_dir = kwargs.get('work_dir', '.')
  include_dir = kwargs.get(
      'include_dir', os.path.join(os.path.dirname(doc_dir), 'include', 'interval_dict'))
  # Build docs.
  cmd = ['doxygen', '-']
  p = Popen(cmd, stdin=PIPE)
  doxyxml_dir = os.path.join(doc_dir, 'doxyxml')
  cmd_str=rf'''
      PROJECT_NAME      = interval_dict
      EXAMPLE_PATH      = {include_dir}/../..
      GENERATE_LATEX    = NO
      GENERATE_MAN      = NO
      GENERATE_RTF      = NO
      CASE_SENSE_NAMES  = NO
      INPUT             = {include_dir}/bi_intervaldict.h \
                          {include_dir}/bi_intervaldicticl.h \
                          {include_dir}/config.h.in \
                          {include_dir}/gregorian.h \
                          {include_dir}/icl_interval_map_adaptor.h \
                          {include_dir}/intervaldict.h \
                          {include_dir}/intervaldicticl.h \
                          {include_dir}/rebase_implementation.h \
                          {include_dir}/interval_traits.h \
                          {include_dir}/ptime.h
      QUIET             = YES
      JAVADOC_AUTOBRIEF = YES
      AUTOLINK_SUPPORT  = NO
      GENERATE_HTML     = NO
      GENERATE_XML      = YES
      XML_OUTPUT        = {doxyxml_dir}
      ALIASES           = "rst=\verbatim embed:rst"
      ALIASES          += "endrst=\endverbatim"
      MACRO_EXPANSION   = YES
    '''.encode('UTF-8')
  p.communicate(input=cmd_str)

  if p.returncode != 0:
    raise CalledProcessError(p.returncode, cmd)
  html_dir = os.path.join(work_dir, 'html')
  main_versions = reversed(versions[-3:])
  check_call(["sphinx-build",
              '-Dbreathe_projects.format=' + os.path.abspath(doxyxml_dir),
              '-Dversion=' + version, '-Drelease=' + version,
              '-Aversion=' + version, '-Aversions=' + ','.join(main_versions),
              '-b', 'html', doc_dir, html_dir])
  try:
    check_call(['lessc', '--clean-css',
                '--include-path=' + os.path.join(doc_dir, 'bootstrap'),
                os.path.join(doc_dir, 'interval_dict.less'),
                os.path.join(html_dir, '_static', 'interval_dict.css')])
  except OSError as e:
    if e.errno != errno.ENOENT:
      raise
    print('lessc not found; make sure that Less (http://lesscss.org/) ' +
          'is installed')
    sys.exit(1)
  return html_dir

if __name__ == '__main__':
  create_build_env()
  build_docs(sys.argv[1], sys.argv[2])
