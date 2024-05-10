
# Arm Platform Security Architecture : Test Suite


## Introduction

This test suite is one of a set of resources provided by Arm that can help organizations develop products that meet the security requirements of PSA Certified on Arm-based platforms. The PSA Certified scheme provides a framework and methodology that helps silicon manufacturers, system software providers and OEMs to develop more secure products. Arm resources that support PSA Certified range from threat models, standard architectures that simplify development and increase portability, and open-source partnerships that provide ready-to-use software. You can read more about PSA Certified here at [https://www.psacertified.org](www.psacertified.org) and find more Arm resources here at [https://developer.arm.com/platform-security-resources](developer.arm.com/platform-security-resources).

For more information, visit the [PSA webpage](https://developer.arm.com/products/architecture/platform-security-architecture)

### PSA Certified APIs specifications

API specifications are placed in [PSA API github site](https://github.com/ARM-software/psa-api/).

### PSA Certified API Compliance
The [PSA Certified API tests](api-tests/dev_apis) are the basis for validating compliance with PSA Certified APIs. For more information on the certification program, see [psacertified.org](https://www.psacertified.org/functional-api-certification/) 

## GitHub branch
  - For API certification, use the release branch and pick the appropriate release tag.
  - To get the latest version of the code with bug fixes and new features, use the master branch.
  
## Architecture test suite

The current implementation of the Architecture test suite contains tests for following PSA Certified specifications. Arm licensees may contact their partner manager to obtain a copy of this specification. <br />

Future versions of the test suite will be enhanced to include tests for other specifications of the Platform Security Architecture.

The tests are available as open source. The tests and the corresponding abstraction layers are available with an Apache v2 license allowing for external contribution.

### TBSA-v8M
The test suite for this specification is located in the tbsa-v8m directory of this repository. See [TBSA-v8m Readme](tbsa-v8m/README.md) file for more details.

### PSA APIs
The test suite for this specification is located in the api-tests directory of this repository. See [PSA APIs Tests Readme](api-tests/README.md) file for more details.

### Secure Debug
The test suite for this specification is located in the secure-debug directory of this repository. See [Secure Debug Readme](secure-debug/README.md) file for more details.

## License

Arm PSA test suite is distributed under Apache v2.0 License.


## Feedback, contributions, and support

 - For feedback, use the GitHub Issue Tracker that is associated with this repository.
 - For support, send an email to support-psa-arch-tests@arm.com with details.
 - Arm licensees can contact Arm directly through their partner managers.
 - Arm welcomes code contributions through GitHub pull requests. 

--------------

*Copyright (c) 2018-2022, Arm Limited and Contributors. All rights reserved.*
