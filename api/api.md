<!-- Generator: Widdershins v4.0.1 -->

<h1 id="opc-ua-testtool-api">OPC UA TestTool API v1.0.0</h1>

> Scroll down for code samples, example requests and responses. Select a language for code samples from the tabs above or the mobile navigation menu.

api of test tool app

Base URLs:

* <a href="https://opcuatesttool.com">https://opcuatesttool.com</a>

Email: <a href="mailto:matkonnerth@gmail.com">Support</a> 

<h1 id="opc-ua-testtool-api-default">Default</h1>

## get__api_jobs

`GET /api/jobs`

*get finished test jobs*

<h3 id="get__api_jobs-parameters">Parameters</h3>

|Name|In|Type|Required|Description|
|---|---|---|---|---|
|from|query|integer|true|start job|
|max|query|integer|false|max number of jobs to return|

> Example responses

> 200 Response

```json
[
  {
    "request": {
      "name": "MyTestJob",
      "script": "browseRecursive.chai",
      "serverUri": "opc.tcp://localhost:4840"
    },
    "result": {
      "id": 0,
      "statusCode": "string",
      "totalRuntime_ms": 0,
      "ts_start": 0,
      "ts_end": 0
    }
  }
]
```

<h3 id="get__api_jobs-responses">Responses</h3>

|Status|Meaning|Description|Schema|
|---|---|---|---|
|200|[OK](https://tools.ietf.org/html/rfc7231#section-6.3.1)|jobs|Inline|
|400|[Bad Request](https://tools.ietf.org/html/rfc7231#section-6.5.1)|bad input parameter|None|

<h3 id="get__api_jobs-responseschema">Response Schema</h3>

Status Code **200**

|Name|Type|Required|Restrictions|Description|
|---|---|---|---|---|
|*anonymous*|[[FinishedJob](#schemafinishedjob)]|false|none|none|
|» request|[JobRequest](#schemajobrequest)|true|none|none|
|»» name|string|true|none|none|
|»» script|string|true|none|none|
|»» serverUri|string|true|none|none|
|» result|object|true|none|none|
|»» id|number|true|none|none|
|»» statusCode|string|true|none|none|
|»» totalRuntime_ms|number|true|none|none|
|»» ts_start|number|true|none|none|
|»» ts_end|number|true|none|none|

<aside class="success">
This operation does not require authentication
</aside>

## post__api_jobs

`POST /api/jobs`

*schedules a new job*

> Body parameter

```json
{
  "name": "MyTestJob",
  "script": "browseRecursive.chai",
  "serverUri": "opc.tcp://localhost:4840"
}
```

<h3 id="post__api_jobs-parameters">Parameters</h3>

|Name|In|Type|Required|Description|
|---|---|---|---|---|
|body|body|[JobRequest](#schemajobrequest)|false|Inventory item to add|

<h3 id="post__api_jobs-responses">Responses</h3>

|Status|Meaning|Description|Schema|
|---|---|---|---|
|201|[Created](https://tools.ietf.org/html/rfc7231#section-6.3.2)|item created|None|
|400|[Bad Request](https://tools.ietf.org/html/rfc7231#section-6.5.1)|invalid input, object invalid|None|
|409|[Conflict](https://tools.ietf.org/html/rfc7231#section-6.5.8)|an existing item already exists|None|

<aside class="success">
This operation does not require authentication
</aside>

## get__api_jobs_{id}

`GET /api/jobs/{id}`

*get finished test job with specified id*

<h3 id="get__api_jobs_{id}-parameters">Parameters</h3>

|Name|In|Type|Required|Description|
|---|---|---|---|---|
|id|path|integer|true|Numeric ID of the finished job to get|

> Example responses

> 200 Response

```json
{
  "request": {
    "name": "MyTestJob",
    "script": "browseRecursive.chai",
    "serverUri": "opc.tcp://localhost:4840"
  },
  "result": {
    "id": 0,
    "statusCode": "string",
    "totalRuntime_ms": 0,
    "ts_start": 0,
    "ts_end": 0
  }
}
```

<h3 id="get__api_jobs_{id}-responses">Responses</h3>

|Status|Meaning|Description|Schema|
|---|---|---|---|
|200|[OK](https://tools.ietf.org/html/rfc7231#section-6.3.1)|Finished Job|[FinishedJob](#schemafinishedjob)|
|400|[Bad Request](https://tools.ietf.org/html/rfc7231#section-6.5.1)|bad input parameter|None|
|500|[Internal Server Error](https://tools.ietf.org/html/rfc7231#section-6.6.1)|job not found|None|

<aside class="success">
This operation does not require authentication
</aside>

## get__api_jobs_{id}_log

`GET /api/jobs/{id}/log`

*get log of finished job*

<h3 id="get__api_jobs_{id}_log-parameters">Parameters</h3>

|Name|In|Type|Required|Description|
|---|---|---|---|---|
|id|path|integer|true|Numeric ID of the finished job to get|

> Example responses

> 200 Response

```
"string"
```

<h3 id="get__api_jobs_{id}_log-responses">Responses</h3>

|Status|Meaning|Description|Schema|
|---|---|---|---|
|200|[OK](https://tools.ietf.org/html/rfc7231#section-6.3.1)|log content|string|
|400|[Bad Request](https://tools.ietf.org/html/rfc7231#section-6.5.1)|bad input parameter|None|
|500|[Internal Server Error](https://tools.ietf.org/html/rfc7231#section-6.6.1)|job not found|None|

<aside class="success">
This operation does not require authentication
</aside>

## get__api_targets

`GET /api/targets`

*get test targets*

> Example responses

> 200 Response

```json
[
  {
    "name": "TestTarget1",
    "host": "localhost",
    "port": 4840
  }
]
```

<h3 id="get__api_targets-responses">Responses</h3>

|Status|Meaning|Description|Schema|
|---|---|---|---|
|200|[OK](https://tools.ietf.org/html/rfc7231#section-6.3.1)|log content|Inline|
|400|[Bad Request](https://tools.ietf.org/html/rfc7231#section-6.5.1)|bad input parameter|None|

<h3 id="get__api_targets-responseschema">Response Schema</h3>

Status Code **200**

|Name|Type|Required|Restrictions|Description|
|---|---|---|---|---|
|*anonymous*|[[Target](#schematarget)]|false|none|none|
|» name|string|true|none|none|
|» host|string|true|none|none|
|» port|number|true|none|none|

<aside class="success">
This operation does not require authentication
</aside>

# Schemas

<h2 id="tocS_JobRequest">JobRequest</h2>
<!-- backwards compatibility -->
<a id="schemajobrequest"></a>
<a id="schema_JobRequest"></a>
<a id="tocSjobrequest"></a>
<a id="tocsjobrequest"></a>

```json
{
  "name": "MyTestJob",
  "script": "browseRecursive.chai",
  "serverUri": "opc.tcp://localhost:4840"
}

```

### Properties

|Name|Type|Required|Restrictions|Description|
|---|---|---|---|---|
|name|string|true|none|none|
|script|string|true|none|none|
|serverUri|string|true|none|none|

<h2 id="tocS_JobResult">JobResult</h2>
<!-- backwards compatibility -->
<a id="schemajobresult"></a>
<a id="schema_JobResult"></a>
<a id="tocSjobresult"></a>
<a id="tocsjobresult"></a>

```json
{
  "id": 0,
  "statusCode": "string",
  "totalRuntime_ms": 0,
  "ts_start": 0,
  "ts_end": 0
}

```

### Properties

|Name|Type|Required|Restrictions|Description|
|---|---|---|---|---|
|id|number|true|none|none|
|statusCode|string|true|none|none|
|totalRuntime_ms|number|true|none|none|
|ts_start|number|true|none|none|
|ts_end|number|true|none|none|

<h2 id="tocS_FinishedJob">FinishedJob</h2>
<!-- backwards compatibility -->
<a id="schemafinishedjob"></a>
<a id="schema_FinishedJob"></a>
<a id="tocSfinishedjob"></a>
<a id="tocsfinishedjob"></a>

```json
{
  "request": {
    "name": "MyTestJob",
    "script": "browseRecursive.chai",
    "serverUri": "opc.tcp://localhost:4840"
  },
  "result": {
    "id": 0,
    "statusCode": "string",
    "totalRuntime_ms": 0,
    "ts_start": 0,
    "ts_end": 0
  }
}

```

### Properties

|Name|Type|Required|Restrictions|Description|
|---|---|---|---|---|
|request|[JobRequest](#schemajobrequest)|true|none|none|
|result|[JobResult](#schemajobresult)|true|none|none|

<h2 id="tocS_Target">Target</h2>
<!-- backwards compatibility -->
<a id="schematarget"></a>
<a id="schema_Target"></a>
<a id="tocStarget"></a>
<a id="tocstarget"></a>

```json
{
  "name": "TestTarget1",
  "host": "localhost",
  "port": 4840
}

```

### Properties

|Name|Type|Required|Restrictions|Description|
|---|---|---|---|---|
|name|string|true|none|none|
|host|string|true|none|none|
|port|number|true|none|none|

