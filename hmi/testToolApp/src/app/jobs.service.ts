import { Injectable, NgZone } from '@angular/core';
import { FinishedJob, FinishedJobsResponse, Request, Result } from './job';
import { Observable, Observer, of } from 'rxjs';
import { HttpClient} from '@angular/common/http';
import { catchError, map } from 'rxjs/operators';

@Injectable({
  providedIn: 'root'
})
export class JobsService {

  constructor(private http: HttpClient, private _zone: NgZone) { }

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

  // getServerSentEvent(): Observable<string> {
  //   return Observable.create((observer : Observer<string>) => {
  //     const eventSource = this.getEventSource('http://localhost:9888/event1');
  //     eventSource.onmessage = event => {
  //       this._zone.run(() => {
  //         observer.next(event);
  //       });
  //     };
  //     eventSource.onerror = error => {
  //       this._zone.run(() => {
  //         observer.error(error);
  //       });
  //     };
  //   });
  // }

  private getEventSource(url: string): EventSource {
    return new EventSource(url);
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