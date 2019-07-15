# quick test

- Physical Layer with hardcoded local fs for reading only
- Provide simple abstractions for the Source/FS - later will be replaced by Arrow abstractions...

## physical layer

there are 3 components
- file header
- records: each consisting of key + blob
- free segments: record consisting of key + blob (list of free segments)

functionality on this layer: given a source and/or sink
- extract all components
 - file header
 - free segments record
 - record after record
 - record at a location + assume not known size of the record
 - record at a location + known size of the record
 - record by key: similar to previous one

## Things to resolve

- conventions for the physical layer
 - empty record
