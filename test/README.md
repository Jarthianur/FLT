# VFR-B Test Reports

This directory contains a C++ unit-test framework as git submodule. In the reports folder, a test report and a HTML coverage report can be found. To view the coverage report open up the index.html in any browser. To view the test report open up the unittest/index.html in any browser.

These unit-tests are part of the travis build process. Thus a passing build includes running tests without any failures or errors.
Some components can not be tested inside a testsuite, hence the coverage for those stays low. But every component is at least tested live in a real, and simulated, environment.

## include test framework

Run `git submodule update --remote`.
