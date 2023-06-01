# Change Log
All notable changes to this project will be documented in this file.
This project adheres to [Semantic Versioning](http://semver.org/).

## [2.0.6] - 2021-04-19
### Fixed
- Support empty node (BU_) list with just Vector__XXX

## [2.0.5] - 2020-01-04
### Changed
- Changed style to Google
- Fixed some clang-tidy warnings

## [2.0.4] - 2019-10-12
### Fixed
- BO_TX_BU is a comma separated list

## [2.0.3] - 2019-06-26
### Changed
- operator<< argument is now const
- C++11 member initializer lists instead of constructors
- operator< separated from class

## [2.0.2] - 2019-03-08
### Fixed
- Support of newest Bison versions

## [2.0.1] - 2019-02-14
### Added
- Support for floating numbers with exponent part
- Support for comment lines
### Changed
- Dropped C++17 specific language features
### Fixed
- Windows line endings in NS jammed the flex scanner
- Added missing return in Signal.cpp

## [2.0.0] - 2019-01-13
### Changed
- Switch from regex to flex/bison
- Changes in Data Model
- Support for operator <<, >>, instead of read/write

## [1.0.6] - 2018-07-31
### Fixed
- Value table description with first character blank fixed

## [1.0.5] - 2018-06-11
### Fixed
- Multiline comments with trailing line feed works now

## [1.0.4] - 2017-12-12
### Changed
- Includes use <> now instead of ""
### Fixed
- Fixed C++11 settings that affected AttributeValueType enum class.

## [1.0.3] - 2017-11-20
### Fixed
- Fixed error in test databaes. BO_TX_BU_ had wrong transmitter seperator.

## [1.0.2] - 2017-09-22
### Changed
- SignalGroup::repetitions is set to 1 by the constructor
- Update to latest project template
### Fixed
- Allow to read files that have messages without blank line as separator

## [1.0.1] - 2016-06-17
### Added
- Compiler hardening flags enabled

## [1.0.0] - 2016-05-20
### Added
- Initial version
