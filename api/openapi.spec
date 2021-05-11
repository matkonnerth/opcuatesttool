openapi: 3.0.0
servers:
  - description: OPC UA TestTool API
    url: https://opcuatesttool.com
info:
  description: This is a simple API
  version: "0.1.0"
  title: OPC UA TestTool API
  contact:
    email: matkonnerth@gmail.com
paths:
  /api/jobs:
    get:
      summary: get finished test jobs
      parameters:
        - in: query
          name: from
          description: start job
          required: true
          schema:
            type: integer
        - in: query
          name: max
          description: max number of jobs to return
          schema:
            type: integer
      responses:
        '200':
          description: jobs
          content:
            application/json:
              schema:
                type: array
                items:
                  $ref: '#/components/schemas/FinishedJob'
        '400':
          description: bad input parameter
    post:
      summary: schedules a new job
      responses:
        '201':
          description: item created
        '400':
          description: 'invalid input, object invalid'
        '409':
          description: an existing item already exists
      requestBody:
        content:
          application/json:
            schema:
              $ref: '#/components/schemas/JobRequest'
        description: Inventory item to add
components:
  schemas:
    JobRequest:
      type: object
      required:
        - name
        - script
        - serverUri
      properties:
        name:
          type: string
          example: 'MyTestJob'
        script:
          type: string
          example: 'browseRecursive.chai'
        serverUri:
          type: string
          example: 'opc.tcp://localhost:4840'
    JobResult:
      required:
        - id
        - statusCode
        - totalRuntime_ms
        - ts_start
        - ts_end
      properties:
        id:
          type: number
        statusCode:
          type: string
        totalRuntime_ms:
          type: number
        ts_start:
          type: number
        ts_end:
          type: number
    FinishedJob:
      required:
        - request
        - result
      properties:
        request:
          $ref: '#/components/schemas/JobRequest'
        result:
          $ref: '#/components/schemas/JobResult'