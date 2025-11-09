cmake = import('cmake')

inc = include_directories([
  './include',
  '../include',
  ])

srcs = [
  './src/example.cpp',
]

deps = [
  ssound_dep,
  sthread_dep,
]

if ['windows'].contains(host_machine.system())

elif ['darwin'].contains(host_machine.system())
  
  cocoa_dep = dependency('cocoa', required: true)
  deps += cocoa_dep
  iokit_dep = dependency('IOKit', required: true)
  deps += iokit_dep

elif ['linux'].contains(host_machine.system())

endif

executable(
  'example',
  srcs,
  install: false,
  include_directories: inc,
  dependencies: deps,
  cpp_args: cpp_define_args,
)

