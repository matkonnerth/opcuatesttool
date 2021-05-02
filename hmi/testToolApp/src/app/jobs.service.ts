import { Injectable } from '@angular/core';
import { Job, FinishedJobsResponse, FinishedJobs } from './job';
import { Observable, of } from 'rxjs';
import { HttpClient, HttpHeaders } from '@angular/common/http';
import { map } from 'rxjs/operators';

@Injectable({
  providedIn: 'root'
})
export class JobsService {

  constructor(private http: HttpClient) { }

  private jobsUrl = 'http://localhost:9888/jobs?from=0&max=10';  // URL to web api

  getJobs(): Observable<Job[]> {
    const jobs = this.http.get<FinishedJobsResponse>(this.jobsUrl).
      pipe(map(response => response.data.map(data => {
        const job: Job = {
          request: data.request,
          result: data.result
        };
        return job;
      })));
    return jobs;
}
}