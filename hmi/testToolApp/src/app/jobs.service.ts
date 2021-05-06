import { Injectable } from '@angular/core';
import { FinishedJob, FinishedJobsResponse, Request, Result } from './job';
import { Observable, of } from 'rxjs';
import { HttpClient} from '@angular/common/http';
import { catchError, map } from 'rxjs/operators';

@Injectable({
  providedIn: 'root'
})
export class JobsService {

  constructor(private http: HttpClient) { }

  private jobsUrl = 'http://localhost:9888/api/jobs';  // URL to web api

  getJobs(): Observable<FinishedJob[]> {
    const jobs = this.http.get<FinishedJobsResponse>(this.jobsUrl + '?from=100000&max=10').
      pipe(map(response => response.response.map(data => {
        const job: FinishedJob = {
          request: data.request,
          result: data.result
        };
        return job;
      })));
    return jobs;
  }

  /** POST: run new test job */
  createJob(job: Request): void {
    const req = this.http.post(this.jobsUrl, job)
      .pipe(
        catchError(this.handleError('createJob', job))
    );
    req.subscribe();
  }

  getJobLog(job: Result): Observable<string>
  {
    const content = this.http.get(this.jobsUrl + '/logs/' + job.id, { responseType: 'text' });
    return content;
  }

  /**
   * Handle Http operation that failed.
   * Let the app continue.
   * @param operation - name of the operation that failed
   * @param result - optional value to return as the observable result
   */
  private handleError<T>(operation = 'operation', result?: T) {
    return (error: any): Observable<T> => {

      // TODO: send the error to remote logging infrastructure
      console.error(operation, error); // log to console instead

      // Let the app keep running by returning an empty result.
      return of(result as T);
    };
  }
}