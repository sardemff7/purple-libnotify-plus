project('purple-libnotify+', 'c',
    version: '2.99.2',
    meson_version: '>=0.58.0',
    license: [ 'GPL3+' ],
    default_options: [
        'c_std=gnu11',
        'warning_level=2',
    ],
)

purple_min_version='2.6.0'
purple_events_min_version='0.99.1'
libnotify_min_version='0.7.0'
gdk_pixbuf_min_version='0.26'
package_developer='sardemff7'

glib = dependency('glib-2.0')
purple = dependency('purple', version: '>= @0@'.format(purple_min_version))
purple_events = dependency('purple-events', version: '>= @0@'.format(purple_events_min_version))
libnotify = dependency('libnotify', version: '>= @0@'.format(libnotify_min_version))
gdk_pixbuf = dependency('gdk-pixbuf-2.0', version: '>= @0@'.format(gdk_pixbuf_min_version))

add_project_arguments(
    '-fvisibility=hidden',
    '-Wno-unused-parameter',
    '-fcommon',
    language: 'c'
)

header_conf = configuration_data()
header_conf.set_quoted('PACKAGE_NAME', meson.project_name())
header_conf.set_quoted('PACKAGE_VERSION', meson.project_version())
header_conf.set_quoted('MODULES_VERSION', meson.project_version())
header_conf.set_quoted('GETTEXT_PACKAGE', meson.project_name())
header_conf.set_quoted('PURPLE_EVENTS_PLUGIN_ID', 'core-@0@-@1@'.format(package_developer, meson.project_name()))
header_conf.set_quoted('PURPLE_DATADIR', purple.get_variable('datadir'))

config_h = configure_file(output: 'config.h', configuration: header_conf)

plugindir = purple.get_variable('plugindir')

shared_module('purple-libnotify+', config_h, files(
        'src/purple-libnotify+-frames.h',
        'src/purple-libnotify+-utils.h',
        'src/purple-libnotify+-common.h',
        'src/purple-libnotify+-frames.c',
        'src/purple-libnotify+-utils.c',
        'src/purple-libnotify+.c',
    ),
    name_prefix: '',
    dependencies: [ glib, purple, purple_events, libnotify, gdk_pixbuf ],
    install: true,
    install_dir: plugindir,
)

subdir('po')
