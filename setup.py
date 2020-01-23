import setuptools

# with open("README.md", "r") as fh:
#    long_description = fh.read()

setuptools.setup(
    name="uws", # Replace with your own username
    version="0.0.1",
    author="Alex Hultman",
    author_email="alexhultman@gmail.com",
    description="First test release of µWebSockets.py",
    #long_description=long_description,
    #long_description_content_type="text/markdown",
    url="https://github.com/uNetworking/uWebSockets.py",
    packages=setuptools.find_packages(),
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
    ],
    python_requires='>=3.6',
    package_data={
        "uws": ["*.so"],
    },
)
