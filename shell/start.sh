#!/bin/bash
(trap 'kill 0' SIGINT; influDBImporter & testService)
