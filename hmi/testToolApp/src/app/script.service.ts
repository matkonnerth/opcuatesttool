import { Injectable } from '@angular/core';
import { Observable, of } from 'rxjs';
import { HttpClient} from '@angular/common/http';
import { catchError, map } from 'rxjs/operators';
import { GetScriptsResponse, Script } from './script';

@Injectable({
  providedIn: 'root'
})
export class ScriptService {

  constructor(private http: HttpClient) { }

  private serviceUrl = 'http://localhost:9888/api/scripts';

  getScripts(): Observable<Script[]> {
    const jobs = this.http.get<GetScriptsResponse>(this.serviceUrl).
      pipe(map(response => response.response.map(data => {
        return data;
      })));
    return jobs;
  }

  getScriptContent(script: Script): Observable<string>
  {
    const content = this.http.get(this.serviceUrl + '/' + script.name, { responseType: 'text' });
    return content;
  }

  /** POST: run new test job */
  updateScript(script: Script, content: string): void {
    const req = this.http.post(this.serviceUrl + '/' + script.name, content);
    req.subscribe();
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
