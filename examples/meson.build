cmake = import('cmake')

inc = include_directories([
  '../include',
  '../subprojects/SThread/include',
  ])

srcs = [
  './src/example.cpp',
]

deps = [
  ssound_dep,
  sthread_dep
]

if ['windows'].contains(host_machine.system())
  winmm_lib = compiler.find_library('winmm', required: true)
  deps += winmm_lib
elif ['darwin'].contains(host_machine.system())
  
  cocoa_dep = dependency('cocoa', required: true)
  deps += cocoa_dep
  iokit_dep = dependency('IOKit', required: true)
  deps += iokit_dep
  audiotoolbox_dep = dependency('AudioToolbox', method: 'extraframework', required: true)
  deps += audiotoolbox_dep

elif ['linux'].contains(host_machine.system())
  alsa_dep = dependency('alsa', required: true)
  deps += alsa_dep
endif


fs = import('fs')
fs.copyfile('assets/sample.wav')


executable(
  'example',
  srcs,
  install: false,
  include_directories: inc,
  dependencies: deps,
  cpp_args: cpp_define_args,
)

