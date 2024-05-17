.. _platform_documentation:

######################
Platform Documentation
######################

This document describes conventions for platform documentation for easy and smooth
integration into TF-M online documentation.

**********
Principles
**********

- Minimise TF-M requirements and maximise flexibility to let platform vendors
  reuse existing documentation.

- Platforms are grouped under vendors' name. There is space to keep general
  information not specific to any of the vendors' platforms.

- A Table Of Contents (TOC) structures platforms' documentation. There is no
  defined template for platform documents.

*****
Rules
*****

- Platforms documentation is gathered in `/docs/platform` folder.
  This folder is similar to the platforms implementation folder
  `/platform/ext/target` but does not need to mirror its structure.

- A vendor is represented by a subfolder with `index.rst` as an entry
  point. This file may contain general materials about a vendor
  not specific to a particular platform.

- A vendor's `index.rst` shall carry a TOCtree structure with a list of all
  supported platforms at the top level.

- An explicit platform name is preferred in a vendor's TOCtree instead of use of
  platform's title by default.

.. note::
  Do not forget to update the summary file after adding a new platform
  `/docs/platform/platform_introduction.rst` included into TF-M introduction.

*******
Example
*******

Assuming a vendor `ABCD123` wants to add and document a new platform
`Secure 1024` in TF-M. Here are the basic steps:

1. Create a folder `/docs/platform/ABCD123/`

2. Create a platform documentation under `/docs/platform/ABCD123`.
   A natural choice is to put a new platform under `/docs/platform/ABCD123/Secure_1024`
   folder but it's not required.

3. Create a file `/docs/platform/ABCD123/index.rst` with a platform entry, presuming
   it's location in `/docs/platform/ABCD123/Secure_1024/index.rst`:

.. code-block:: rst

  #################
  ABCD123 platforms
  #################

  .. toctree::
  Secure 1024 <Secure_1024/index>

4. Finally, update `/docs/platform/index.rst` with a new vendor name:

.. code-block:: rst

  ##############
  TF-M Platforms
  ##############

  .. toctree::
      :maxdepth: 2
      :glob:

      Arm <arm/index.rst>
      NXP <nxp/index.rst>
      Cypress <cypress/index.rst>
      ...
      ABCD123 <ABCD123/index.rst>

--------------

*Copyright (c) 2023, Arm Limited. All rights reserved.*
