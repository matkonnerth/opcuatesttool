# WIP: opcuaTestTool

## Architecture

Components in green are covered by this project. A http client can trigger new jobs, retrieve results. After requesting a new job, the TestService forks and executes the TestRunner. The TestRunner is a single threaded application, with parses the job configuration, connects to the OPC UA Server and issues the tests. After finishing the tests, a result file with json content is written.

![Alt](drawio/architecture.png)

## Useful commands

### Triggering a new job
curl -H "Content-Type: application/json" --data @../tests/rest/newJob.json http://localhost:9080/jobs

### Get finished jobs
curl http://localhost:9080/jobs?finished=true

### Get results of job with id 0
curl http://localhost:9080/jobs/0

### some good reads
https://florimond.dev/blog/articles/2018/08/restful-api-design-13-best-practices-to-make-your-users-happy/



