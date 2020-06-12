# WIP: opcuaTestTool

## Architecture

Components in green are covered by this project. A http client can trigger new jobs, retrieve results. After requesting a new job, the TestService forks and executes the TestRunner. The TestRunner is a single threaded application, which parses the job configuration, connects to the OPC UA Server and executes the requests. After finishing the tests, a result file with json content is written.

![Alt](drawio/architecture.png)

## Jobs and Tasks

A job represents a single test run and contains 1-n tasks. Types of jobs:
* repetiveJob: runs for N iterations
* oneCycleJob: runs the tasks exactly once

A task is a single command, e.g. a read request or a wait. Types of tasks:
* readRequest: reads the value attribute of a node
* browseRequest: browses a node
* wait: pauses the job execution for N milliseconds

## Useful commands

### Triggering a new job
curl -H "Content-Type: application/json" --data @tests/rest/newJob.json http://localhost:9080/jobs

### Get finished jobs
curl http://localhost:9080/jobs?finished=true

### Get results of job with id 0
curl http://localhost:9080/jobs/0

### some good reads
https://florimond.dev/blog/articles/2018/08/restful-api-design-13-best-practices-to-make-your-users-happy/



