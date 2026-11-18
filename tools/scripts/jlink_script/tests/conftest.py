def pytest_addoption(parser):
    parser.addoption("--swd-cert", default=None, help="a built app_cert.bin that carries swd_auth_public_key")
    parser.addoption("--swd-pub", default=None, help="the swd_auth_public_key (X||Y hex) that cert was built with")
