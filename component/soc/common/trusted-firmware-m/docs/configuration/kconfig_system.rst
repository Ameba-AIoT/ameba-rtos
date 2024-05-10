.. _Kconfig_system:

##################
The Kconfig System
##################
The Kconfig system is an alternative tool to the CMake config system for users to change config
options of TF-M.

.. figure:: kconfig_header_file_system.svg

It handles dependencies and validations automatically when you change configurations so that the
generated configuration options are always valid.

To use the Kconfig system, enable ``USE_KCONFIG_TOOL`` in commande line.
And enable ``MENUCONFIG`` to launch configuration GUI.

The Kconfig system consists of `The Kconfig tool`_ and the `The Kconfig files`_.

****************
The Kconfig tool
****************
The Kconfig tool is a python script based on `Kconfiglib <https://github.com/ulfalizer/Kconfiglib>`__
to generate the following config files:

- CMake config file

  Contains CMake cache variables of building options.

- Header file

  Contains component options in the header file system.
  Component options are gathered together in a seperate menu ``TF-M component configs`` in
  `The Kconfig files`_.

- The .config and .config.old files

  The ``.config`` file which contains all the above configurations in the Kconfig format.
  It will be created after the first execution of the script. It is only used to allow
  users to make adjustments basing on the previous settings.
  The Kconfig tool will load it if it exists and ``.config.old`` will be created to
  save the previous configurations.

The tool supports loading multiple pre-set configuration files merging into a single one.
The first loaded options are overridden by later ones if the config files contain duplicated
options.
And dependencies between config options are taken care of.
It then launchs a configuration GUI for users to change any config options if the ``MENUCONFIG`` is
enabled in build command line.

Integration with TF-M build system
----------------------------------
TF-M build system includes ``kconfig.cmake`` to integrate this tool.
It prepares the parameters for the script and invokes it to load multiple configuration files basing
on your build setup, including but not limited to

  - Build type bound configurations, decided by ``CMAKE_BUILD_TYPE``
  - Profile configurations, decided by ``TFM_PROFILE``

**************************
Customizing config options
**************************
By default, the Kconfig system only merges configuration files and generated the final config files.
To customize the config options, there are several approaches.

Menuconfig
----------
Menuconfig is the recommended approach to adjust the values of the config options because it has
a graphic interface for you to easily change the options without worrying about dependencies.

To launch the menuconfig, you need to enable ``MENUCONFIG`` in addition to enabling
``USE_KCONFIG_TOOL``.

.. code-block:: bash

  cmake -S . -B cmake_build -DTFM_PLATFORM=arm/mps2/an521 \
                            -DUSE_KCONFIG_TOOL=ON \
                            -DMENUCONFIG=ON

.. note::

  Although the Kconfiglib provides three
  `menuconfig interfaces <https://github.com/ulfalizer/Kconfiglib#menuconfig-interfaces>`__,
  only GUI menuconfig can be launched by CMake for the time being.

Command line options
--------------------
The support of passing configurations via command line is kept for the Kconfig system.

.. code-block:: bash

  cmake -S . -B cmake_build -DTFM_PLATFORM=arm/mps2/an521 \
                            -DUSE_KCONFIG_TOOL=ON \
                            -DTFM_ISOLATION_LEVEL=2

Kconfig file
------------
You can also put the frequently used config options into a Kconfig file. When you need to apply the
config options in that file, pass it via command line option ``-DKCONFIG_CONFIG_FILE``

.. code-block:: bash

  cmake -S . -B cmake_build -DTFM_PLATFORM=arm/mps2/an521 \
                            -DTFM_ISOLATION_LEVEL=2 \
                            -DUSE_KCONFIG_TOOL=ON \
                            -DKCONFIG_CONFIG_FILE=my_config.conf

.. note::

  The command line set options override the ones in the config file.
  And you can always launch menuconfig to do the final adjustments.

*****************
The Kconfig files
*****************
The Kconfig files are the files written by the
`Kconfig language <https://www.kernel.org/doc/html/latest/kbuild/kconfig-language.html#kconfig-language>`__
to describe config options.
They also uses some Kconfiglib extensions such as optional source ``osource`` and relative source ``rsource``
so they can only work with the Kconfiglib.

--------------

*Copyright (c) 2022-2023, Arm Limited. All rights reserved.*
