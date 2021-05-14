import { Injectable, NgZone } from '@angular/core';
import { FinishedJob, FinishedJobsResponse, Request, Result } from './job';
import { Observable, of, Observer } from 'rxjs';
import { HttpClient} from '@angular/common/http';
import { catchError, map } from 'rxjs/operators';
import { environment } from '../environments/environment';
@Injectable({
  providedIn: 'root'
})
export class JobsService {

  constructor(private http: HttpClient, private _zone: NgZone) {
  }

  private baseUrl = environment.baseURL + 'api/jobs';

  getJobs(): Observable<FinishedJob[]> {
    const jobs = this.http.get<FinishedJobsResponse>(this.baseUrl + '?from=100000&max=10').
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
    const req = this.http.post(this.baseUrl, job)
      .pipe(
        catchError(this.handleError('createJob', job))
    );
    req.subscribe();
  }

  getJobLog(job: Result): Observable<string>
  {
    const content = this.http.get(this.baseUrl + '/' + job.id + '/log', { responseType: 'text' });
    return content;
  }

  getServerSentEvent(): Observable<string> {
    return new Observable((observer: Observer<string>) => {
      const eventSource = this.getEventSource(environment.baseURL + 'api/events');
      eventSource.onmessage = event => {
        this._zone.run(() => {
          observer.next(event.data);
        });
      };
      eventSource.onerror = error => {
        this._zone.run(() => {
          observer.error(error);
        });
      };
    });
  }

  getEventSource(uri: string): EventSource
  {
    return new EventSource(uri);
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
