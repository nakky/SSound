
ssound_incs = [
  '../include',
  '../subprojects/SThread/include',
  ]

ssound_srcs = [
  'Common.cpp',
  'AudioFile.cpp',
  'FileIO.cpp',
  'SoundCommon.cpp',
  'SoundBuffer.cpp',
  'SoundDriver.cpp',
  'SoundEffector.cpp',
  'SoundFactory.cpp',
  'SoundFileDescriptor.cpp',
  'SoundSystem.cpp',
  'SoundObject.cpp',
  'WavFileCreator.cpp',
]

if ['windows'].contains(host_machine.system())
  ssound_srcs += 'MME/MMEDriver.cpp'
elif ['darwin'].contains(host_machine.system())
  ssound_srcs += 'CoreAudio/CoreAudioDriver.mm'
elif ['linux'].contains(host_machine.system())
  ssound_incs += '/usr/include'
  ssound_srcs += 'ALSA/ALSADriver.cpp'
endif

sthread_dep = dependency(
  'SThread',
  version: ['>= 0.1.0'],
  fallback: ['SThread', 'sthread_dep'],
  default_options: ['default_library=static', 'examples=false'],
)

deps = [
  sthread_dep,
]

ssound_inc = include_directories(ssound_incs)

ssound_lib = static_library('ssound',
                ssound_srcs,
                install: true,
                include_directories: ssound_inc,
                dependencies: deps,
                cpp_args: cpp_define_args
                )
